static char *sccsid = "@(#)LD-decay.c  2021-05-27  A.J.Travis";

/*
 * Calculate LD decay using PLINK
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>

#define DEBUG 1
#define NBINS 100
#define KB 1000

void usage();

int main(int argc, char *argv[])
{
    int opt;
    char file[BUFSIZ];
    char out[BUFSIZ];
    int chr = 0;
    int qtl = 0;
    int window = 1000;
    int from_kb;
    int to_kb;
    int ld_window = 99999;
    int ld_window_kb = 5000;
    int ld_window_r2 = 0;
    int scale = 10;
    char filter[BUFSIZ] = "";
    char cmd[BUFSIZ];

    FILE *input;
    FILE *output;
    char *header;
    char *line = NULL;
    unsigned long ld_fq[NBINS];
    double ld_r2[NBINS];
    double ld_dp[NBINS];
    float mean_r2;
    float mean_dp;

    size_t len = 0;
    unsigned long bp_a;
    unsigned long bp_b;
    unsigned long dist;
    unsigned long bin;
    unsigned long n;
    float r2;
    float dp;

    /* get command-line arguments from argv */
    while ((opt = getopt(argc, argv, "c:hq:s:vw:")) != -1) {
	switch (opt) {
	case 'c':
	    chr = atoi(optarg);
	    break;
	case 'h':
	    usage();
	    break;
	case 'q':

	    /* qtl (Kb) */
	    qtl = atoi(optarg);
	    break;
	case 's':

	    /* bin scale (Kb) */
	    scale = atoi(optarg);
	    break;
	case 'v':

	    /* version */
	    fprintf(stderr, "%s\n", sccsid);
	    exit(EXIT_SUCCESS);
	case 'w':

	    /* window (Kb) */
	    window = atoi(optarg);
	    break;
	default:
	    usage();
	}
    }
    if (optind >= argc) {
	usage();
    }
    strcpy(file, argv[optind]);

    /* run PLINK to calculate LD matrix */
    if (chr == 0) {

	/* LD decay only estimated within one chromosome */
	fprintf(stderr, "LD-decay: Error - Chromosome not selected\n");
	usage();
    } else if (qtl == 0) {

	/* entire chromosome */
	sprintf(out, "%s_chr%d", file, chr);
	sprintf(filter, "--chr %d", chr);
    } else {

	/* specific QTL */
	from_kb = qtl - window / 2;
	to_kb = qtl + window / 2;
	sprintf(out, "%s_chr%d-%dk-%dk", file, chr, from_kb, to_kb);
	sprintf(filter, "--chr %d --from-kb %d --to-kb %d", chr,
		from_kb, to_kb);
    }
    sprintf(cmd,
	    "plink1.9 --file %s --out %s --r2 dprime --ld-window-kb %d --ld-window %d --ld-window-r2 %d %s",
	    file, out, ld_window_kb, ld_window, ld_window_r2, filter);
#if DEBUG
    fprintf(stderr, "%s\n", cmd);
#endif
    if (system(cmd) != 0) {
	fprintf(stderr, "LD-decay: Failed to run %s\n", cmd);
	exit(EXIT_FAILURE);
    }

    /* open LD output file created by PLINK */
    sprintf(file, "%s.ld", out);
    if ((input = fopen(file, "r")) == NULL) {

	exit(EXIT_FAILURE);
    }

    /* initialise bins */
    for (bin = 0; bin < NBINS; bin++) {
	ld_fq[bin] = 0;
	ld_r2[bin] = 0;
	ld_dp[bin] = 0;
    }

    /* skip header */
    n = getline(&header, &len, input);

    /* bin LD values by distance */
    printf("LD-decay: binning LD values by distance\n");
    while (getline(&line, &len, input) != -1) {
	sscanf(line, "%*s %ld %*s %*s %ld %*s %f %f", &bp_a, &bp_b, &r2,
	       &dp);
	dist = bp_b - bp_a;
	bin = dist / KB / scale;
	if (bin < NBINS) {
	    ld_fq[bin]++;
	    ld_r2[bin] += r2;
	    ld_dp[bin] += dp;
	}
    }
    fclose(input);

    /* remove PLINK .ld and .nosex files */
    unlink(file);
    sprintf(file, "%s.nosex", out);
    unlink(file);

    /* save histogram to file */
    sprintf(file, "%s.hist", out);
    printf("LD-decay: saving histogram to %s\n", file);
    if ((output = fopen(file, "w")) == NULL) {
	fprintf(stderr, "Can't open %s\n", file);
	exit(EXIT_FAILURE);
    }
    fprintf(output, "%4s\t%s\t%s\t%s\n", "Kb", "R2", "DP", "N");
    for (bin = 0; bin < NBINS; bin++) {
	n = ld_fq[bin];
	if (n == 0) {
	    mean_r2 = 0;
	    mean_dp = 0;
	} else {
	    mean_r2 = ld_r2[bin] / n;
	    mean_dp = ld_dp[bin] / n;
	}
	fprintf(output, "%4ld\t%0.3f\t%0.3f\t%lu\n", (bin + 1) * scale,
		mean_r2, mean_dp, n);
    }
    fclose(output);

    /* plot histogram */
    sprintf(file, "%s.png", out);
    printf("LD-decay: plotting histogram to %s\n", file);
    sprintf(cmd, "gnuplot -e \"\
	    set terminal png large; \
	    prefix='%s'; \
	    input=prefix.'.hist'; set title prefix; \
	    set output prefix.'.png'; \
	    set key autotitle columnhead; \
	    set yrange [0:]; \
	    set arrow from graph 0,first 0.2 to graph 1,first 0.2 nohead lt 0; \
	    plot input using 1:2 with lines\"", out);
    if (system(cmd) != 0) {
	fprintf(stderr, "LD-decay: Failed to run %s\n", cmd);
	exit(EXIT_FAILURE);
    }
    return (EXIT_SUCCESS);
}

/*
 * Display correct usage then exit
 */
void usage()
{
    fprintf(stderr, "\nUsage:\n");
    fprintf(stderr,
	    "    LD-decay -c number [-q qtl_kb] [-s scale_kb] [-v] [-w window_kb] prefix\n\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "    -c = chromosome (number)\n");
    fprintf(stderr, "    -h = help\n");
    fprintf(stderr,
	    "    -q = QTL position (qtl_kb), default = entire chromosome\n");
    fprintf(stderr, "    -s = bin size (scale_kb), default = 10Kb\n");
    fprintf(stderr, "    -v = display program version\n");
    fprintf(stderr,
	    "    -w = QTL window size (window_kb), default = 1000\n\n");
    fprintf(stderr, "Files:\n");
    fprintf(stderr, "    prefix = prefix of PLINK .ped and .map files\n");
    fprintf(stderr, "    input = PLINK prefix.ped and prefix.map\n");
    fprintf(stderr,
	    "    output = prefix_chr<n>_<from_kb>_<to_kb>.hist\n\n");
    exit(EXIT_FAILURE);
}

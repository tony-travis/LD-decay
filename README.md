# LD-decay
Calculate and plot the average decay in LD with distance between SNPs

The "LD-decay" program calculates LD (Linkage disequilibrium) using PLINK and
estimates local LD decay by 'binning' r<sup>2</sup> values between pairs of SNP's according
to their distance apart.
~~~~
$ LD-decay -h

Usage:
    LD-decay -c number [-q qtl_kb] [-s scale_kb] [-v] [-w window_kb] prefix

Where:
    -c = chromosome (number)
    -h = help
    -q = QTL position (qtl_kb), default = entire chromosome
    -s = bin size (scale_kb), default = 10Kb
    -v = display program version
    -w = QTL window size (window_kb), default = 1000

Files:
    prefix = prefix of PLINK .ped and .map files
    input = PLINK prefix.ped and prefix.map
    output = prefix_chr<n>_<from_kb>_<to_kb>.hist
~~~~
# extract_snps
Extract original (non-recoded) SNPS from genotype file around QTL of interest

The "extract_snps" program uses PLINK to extract the original SNP's that were recoded to do GWAS using the PIQUE pipeline.
~~~~
Usage:
    extract_snps GWAS genotype phenotype "PLINK-options"

Where:
    GWAS = full path to GWAS results folder
    genotype = full path prefix to .ped and .map files used for GWAS
    phenotype = phenotype of interest
    "PLINK-options" = PLINK filter options to select SNP's of interest

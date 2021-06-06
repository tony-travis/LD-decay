#@(#)Makefile  2021-06-06  A.J.Travis

#
# LD-decay
#

WGET = wget --no-check-certificate
LD-DECAY = LD-decay
EXTRACT_SNPS = extract_snps
GENOTYPE = sativas413
TARGETS = $(LD-DECAY) $(EXTRACT_SNPS)

help:
	@echo "Targets:"
	@echo "\tall"
	@echo "\ttest"

all: $(TARGETS)

$(LD-DECAY): LD-decay.c
	cc -Wall -O -o $@ $<

$(GENOTYPE).ped $(GENOTYPE).map:
	$(WGET) http://ricediversity.org/data/sets/44kgwas/RiceDiversity.44K.MSU6.Genotypes_PLINK.zip
	unzip RiceDiversity.44K.MSU6.Genotypes_PLINK.zip
	mv -i ./RiceDiversity_44K_Genotypes_PLINK/sativas* .
	rmdir RiceDiversity_44K_Genotypes_PLINK
	rm RiceDiversity.44K.MSU6.Genotypes_PLINK.zip

test: $(LD-DECAY) $(GENOTYPE).ped $(GENOTYPE).map
	./$< -c 1 -s 50 $(GENOTYPE)
	./$< -c 2 -s 50 $(GENOTYPE)
	./$< -c 3 -s 50 $(GENOTYPE)
	./$< -c 4 -s 50 $(GENOTYPE)
	./$< -c 5 -s 50 $(GENOTYPE)
	./$< -c 6 -s 50 $(GENOTYPE)
	./$< -c 7 -s 50 $(GENOTYPE)
	./$< -c 8 -s 50 $(GENOTYPE)
	./$< -c 9 -s 50 $(GENOTYPE)
	./$< -c 10 -s 50 $(GENOTYPE)
	./$< -c 11 -s 50 $(GENOTYPE)
	./$< -c 12 -s 50 $(GENOTYPE)

install: $(TARGETS)
	cp -ai $^ /usr/local/bin/

clean:
	rm -rf RiceDiversity_44K_Genotypes_PLINK sativas413*
	rm -rf __MACOSX
	rm -f *.o *.log *.hist *.png .pversion

clobber: clean
	rm -f $(LD-DECAY)

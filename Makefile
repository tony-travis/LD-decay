#@(#)Makefile  2021-05-26  A.J.Travis

#
# LD-decay
#

LD-DECAY = LD-decay
GENOTYPE = sativas413

$(LD-DECAY): LD-decay.c
	cc -Wall -O -o $@ $<

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

install: $(LD-DECAY)
	cp -ai $< /usr/local/bin/

clean:
	rm -f *.o *.log *.hist *.png .pversion

clobber: clean
	rm -f $(LD-DECAY)

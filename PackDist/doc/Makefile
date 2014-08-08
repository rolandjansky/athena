# File: PackDist/doc/Makefile
# Author: Grigori Rybkine <Grigori.Rybkine@cern.ch>

# Build documentation: man page and in html format

name = proj-run.sh
script = ../scripts/$(name)
man = ../man/man1/$(name).1
html = ../doc/html/$(name).html

.PHONY: all 

all : $(man) $(html)

$(man) : $(script) ../ChangeLog
	@mkdir -p $(@D)
	help2man -o $@ -n "pack project(s), external package(s)" \
	-m "Packaging and Distribution Tools" -N -h -h -v -v $<

$(html) : $(man)
	@mkdir -p $(@D)
	@rm -f $@
	man2html $< >$@

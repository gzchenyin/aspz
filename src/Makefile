#Makefile

# Created on: Jul 15, 2016
#     Author: ychen

CC = g++
MINISAT_MTL_INCLUDE = . 
CFLAGS = -Wall -Wno-parentheses -I$(MINISAT_MTL_INCLUDE)-pipe -fno-strict-aliasing -ggdb

#ASPZ
ASPZ_HEADER = minisat-Solver.h \
	minisat-SolverTypes.h \
	asp-program.h \

ASPZ_OBJECT = minisat-Solver.o \
	asp-program.o \
	main.o \
	utils/Options.o \
	utils/System.o \

#ASPZ_SOURCE = $(ASPZ_OBJECT:.o=.cc)

############

aspz : $(ASPZ_OBJECT)
	$(CC) -o aspz $(ASPZ_OBJECT)
	
$(ASPZ_OBJECT) : $(ASPZ_HEADER)

.cc.o:
	$(CC) $< $(CFLAGS) -c -o $@

clean :
	rm -f *.o
	rm -f aspz
	


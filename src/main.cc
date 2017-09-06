/*
 * main.cc
 *
 *  Created on: July 15, 2016
 *      Author: ychen
 */

#include <signal.h>
#include <iostream>
#include <fstream>
#include "asp-program.h"

#include "utils/System.h"

using namespace std;
using namespace aspz;

AspProgram *pasp;
Minisat::Solver *psat;

//copy from minisat

void printStats(Minisat::Solver& solver)
{
    double cpu_time = Minisat::cpuTime();
    double mem_used = Minisat::memUsedPeak();
    printf("restarts              : %"PRIu64"\n", solver.starts);
    printf("conflicts             : %-12"PRIu64"   (%.0f /sec)\n", solver.conflicts   , solver.conflicts   /cpu_time);
    printf("decisions             : %-12"PRIu64"   (%4.2f %% random) (%.0f /sec)\n", solver.decisions, (float)solver.rnd_decisions*100 / (float)solver.decisions, solver.decisions   /cpu_time);
    printf("propagations          : %-12"PRIu64"   (%.0f /sec)\n", solver.propagations, solver.propagations/cpu_time);
    printf("conflict literals     : %-12"PRIu64"   (%4.2f %% deleted)\n", solver.tot_literals, (solver.max_literals - solver.tot_literals)*100 / (double)solver.max_literals);
    if (mem_used != 0) printf("Memory used           : %.2f MB\n", mem_used);
    printf("CPU time              : %g s\n", cpu_time);
}

// Terminate by notifying the solver and back out gracefully. This is mainly to have a test-case
// for this feature of the Solver as it may take longer than an immediate call to '_exit()'.
static void SIGINT_interrupt(int signum) 
{
  pasp->interrupt(); 
}

// Note that '_exit()' rather than 'exit()' has to be used. The reason is that 'exit()' calls
// destructors and may cause deadlocks if a malloc/free function happens to be running (these
// functions are guarded by locks for multithreaded use).
static void SIGINT_exit(int signum) 
{
    printf("\n"); printf("*** INTERRUPTED ***\n");
    if (psat->verbosity > 0){
        printStats(*psat);
        printf("\n"); printf("*** INTERRUPTED ***\n"); }
    _exit(1);  
}

int main(int argc, char** argv)
{
  signal(SIGINT, SIGINT_exit);
  signal(SIGXCPU,SIGINT_exit);
  
  ifstream inf;
  
  if (argc <= 1)
  {
    cout << "please input the file name." << endl;
    return 0;
  }
  
  inf.open(argv[1]);
  if (!inf.good())
  {
    cout << "open file error: " << argv[1] << "." << endl;
    return 0;
  }
  
  AspProgram p;
  pasp = &p;
  psat = &(p.sat_solver);
  
  p.read(inf);
  
  signal(SIGINT, SIGINT_interrupt);
  signal(SIGXCPU,SIGINT_interrupt);

  lbool ret = p.asp_solve();
  
  if (ret == l_True)
  {
    cout << "SAT" << endl;
    p.get_model();
  }
  else if (ret == l_False)
    cout << "UNSAT" << endl;
  else
    cout << "INDET" << endl;

  p.dump();
  
  return 0;
}



#ifndef _TRACER_H_
#define _TRACER_H_

#include "utils.h"
#include <assert.h>
/////////////////////////////////////////
/////////////////////////////////////////




#define OPTYPE_LOAD 0
#define OPTYPE_STORE 1
#define OPTYPE_OP 2
#define OPTYPE_CALL_DIRECT 3
#define OPTYPE_RET 4
#define OPTYPE_BRANCH_UNCOND 5
#define OPTYPE_INDIRECT_BR_CALL 6
#define OPTYPE_INDIRECT_BR_CALL 7
#define OPTYPE_MAX 8
#define OpType int
/////////////////////////////////////////
/////////////////////////////////////////

class CBP_TRACE_RECORD{
  public:
  UINT32   PC;
  OpType   opType;
  bool     branchTaken;
  UINT32   branchTarget;

  CBP_TRACE_RECORD(){
    PC=0;
    opType=OPTYPE_MAX;
    branchTaken=0;
    branchTarget=0;
  }
};


/////////////////////////////////////////
/////////////////////////////////////////

class CBP_TRACER{
 private:
  FILE *traceFile;

  UINT64 numInst;        
  UINT64 numCondBranch;

  UINT64 lastHeartBeat;

 public:
  CBP_TRACER(char *traceFileName){
  char  cmdString[1024];
  
  sprintf(cmdString,"gunzip -c %s", traceFileName);

  if ((traceFile = popen(cmdString, "r")) == NULL){
   printf("Unable to open the trace file. Dying\n");
   exit(-1);
  }

  numInst=0;
  numCondBranch=0;

}

  bool   GetNextRecord(CBP_TRACE_RECORD *rec){

  fread (&rec->PC, 4, 1, traceFile);
  fread (&rec->branchTarget, 4, 1, traceFile);
  fread (&rec->opType, 1, 1, traceFile);
  fread (&rec->branchTaken, 1, 1, traceFile);

  if(feof(traceFile)){
    return FAILURE; 
  }

  // sanity check
  assert(rec->opType < 8);

  // update trace stats and heartbeat
  numInst++;
  CheckHeartBeat();

  if(rec->opType == 8){
    numCondBranch++;
  }

  return SUCCESS; 
}

  UINT64 GetNumInst(){ return numInst; }
  UINT64 GetNumCondBranch(){ return numCondBranch; }

 private:
  void   CheckHeartBeat(){
  UINT64 dotInterval=1000000;
  UINT64 lineInterval=30*dotInterval;

  if(numInst-lastHeartBeat >= dotInterval){
    printf("."); 
    fflush(stdout);

    lastHeartBeat=numInst;

    if(numInst % lineInterval == 0){
      printf("\n");
      fflush(stdout);
    }

  }

}
};


/////////////////////////////////////////
/////////////////////////////////////////


#endif // _TRACER_H_


#ifndef _PREDICTOR_H_
#define _PREDICTOR_H_

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <iostream>
#include "utils.h"


#define BPSIZE 4096
#define BPINDEXBITS 12

#define BANKSIZE 1024
#define BANKINDEXBITS 10
#define NUMBANKS 5
#define TAGBITS 8

#define MAXSAT 7
#define UMAX 3
#define PREDMSB 4

#define TAKEN 1
#define NOTTAKEN 0

#define LOOP

#define LPSIZE 1024
#define LPINDEXBITS 10
#define LPTAGBITS 16
#define INITAGE 20
#define HIGHCONFIDENCE 3
#define LOWCONFIDENCE 1

//NOTE competitors are allowed to change anything in this file include the following two defines
//ver2 #define FILTER_UPDATES_USING_BTB     0     //if 1 then the BTB updates are filtered for a given branch if its marker is not btbDYN
//ver2 #define FILTER_PREDICTIONS_USING_BTB 0     //if 1 then static predictions are made (btbANSF=1 -> not-taken, btbATSF=1->taken, btbDYN->use conditional predictor)

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

typedef struct bank_entry{
uint8_t pred_ctr;
uint8_t tag;
uint8_t useful_ctr;
}bank_entry;

typedef struct loop_entry{
  uint16_t loop_count;
  uint16_t iter_count;
  uint16_t tag;
  uint8_t confidence;
  uint16_t age;
  int miss_count;
};


class BasePredictor{
private: 
  uint8_t bp_table[BPSIZE];

public:

BasePredictor(void);

//BASE PREDICTOR METHODS
void init_bp();
void bp_incr(int index);
void bp_decr(int index);
void bp_update(int index, bool resolveDir);
bool bp_getPred(int index);
UINT64 bp_getIndex(UINT64 PC);
};


class Banks{

  private:
    bank_entry bank_array[NUMBANKS][BANKSIZE];
    
  public:
  Banks(void);

  void bank_init();
  bool tag_match(int bank, int entry, int tag);
  bool get_pred(int bank, int entry);
  void incr_pred_ctr(int bank, int entry);
  void decr_pred_ctr(int bank, int entry);
  void set_pred_ctr(int bank, int entry);
  void incr_u_ctr(int bank, int entry);
  void decr_u_ctr(int bank, int entry);
  void clear_u_ctr(int bank, int entry);
  void clearMSBs();
  void clearLSBs();
  void bank_update(int bankno, int entry, bool resolveDir);
  void set_tag(int bankno, int entry, int tag);
  int get_u_ctr(int bank, int entry);
  int get_tag(int bank, int entry);

};

class GHR{


  private:
  __uint128_t ghr;

  public:
  GHR(void);

  void ghr_init();
  void ghr_update(bool resolveDir);
  __uint128_t get_ghr();

};

class LoopPredictor{
  private:
    loop_entry loop_table[LPSIZE];

  public:
    LoopPredictor(void);

    void init_lp();
    int get_prediction(UINT64 PC);
    uint16_t get_tag(UINT64 PC);
    uint16_t get_index(UINT64 PC);
    void incr_confidence(uint16_t index);
    void decr_confidence(uint16_t index);
    void incr_age(uint16_t index);
    void decr_age(uint16_t index);
    void UpdatePredictor(UINT64 PC, bool resolveDir, bool predDir, bool usedlp);

};

class PREDICTOR{

  // The state is defined for Gshare, change for your design

 private:
 // UINT32  ghr;           // global history register
  //UINT32  *pht;          // pattern history table
  //UINT32  historyLength; // history length
  //UINT32  numPhtEntries; // entries in pht 
  GHR ghr;
  BasePredictor bp;
  Banks banks;
  LoopPredictor lp;
  int predno, altpredno;
  bool pred, altpred, usedlp;
  uint16_t index;
  long numbranches; 
  int update_banks[NUMBANKS];
  //int bank_used[NUMBANKS + 2];

 public:

  PREDICTOR(void);
  //NOTE contestants are NOT allowed to use these versions of the functions
//ver2   bool    GetPrediction(UINT64 PC, bool btbANSF, bool btbATSF, bool btbDYN);  
//ver2   void    UpdatePredictor(UINT64 PC, OpType opType, bool resolveDir, bool predDir, UINT64 branchTarget, bool btbANSF, bool btbATSF, bool btbDYN);
//ver2   void    TrackOtherInst(UINT64 PC, OpType opType, bool branchDir, UINT64 branchTarget);

  // The interface to the functions below CAN NOT be changed
  bool    GetPrediction(UINT64 PC);  
  void    UpdatePredictor(UINT64 PC, OpType opType, bool resolveDir, bool predDir, UINT64 branchTarget);
  void    TrackOtherInst(UINT64 PC, OpType opType, bool branchDir, UINT64 branchTarget);

  uint16_t get_bank_index(UINT64 PC, uint8_t bankno, __uint128_t ghr);
  uint16_t get_tag(UINT64 PC, __uint128_t ghr, int bankno);
  void init_update_banks();
  void init_used_banks();
  void print100(const char* str);
  // Contestants can define their own functions below
};
#endif


#include "scheduler.h"
#include "common.h"
#include <stdio.h>
#include <vector>
#include <cmath>

// this function to return the number of JUMPs or skips that we can perform for all ps in the RQ
int64_t skipCaseI(int64_t AT, int64_t TS, int64_t CT, std::vector<std::pair<int, int64_t>> &RQ){
    // I am using AT as the upper bound for this function
    // AT : is the Arrival time of the next process
    // TS : the time slice
    // CT : current time
    // a referrence to RQ
    int64_t i = 0, size = RQ.size();
    int64_t j = floor( (AT-CT) /size);
            j = j/TS;
    int64_t k = TS * j;

    while(1){

        if(i == size){
          // return HOW MANY qunatums(or time slice) is allowed to use
            return j;
        }
        if(RQ[i].second-k>0) i++; // continue
        else{
          // if one of the processes failed
          // the try a lower time slice by 1
          i = 0; j--; k = TS * j;
        }
        // if k is 0 the perform 1 skip
        if(k==0) return 1;
    }
    return 1;
}


void simulate_rr(
  int64_t quantum,
  int64_t max_seq_len,
  std::vector < Process > & processes,
  std::vector < int > & seq
  ) {
  seq.clear();
  // ATF : Arrival time of the next process
  // RV : a return value from a function
  // ITER variable is for adding proceses to the seq
  // The rest of varibale can be considered as temp
  int64_t ATF = -1, RV = 0, i = 0, j = 0, k=0, t=0, ITER = 0;
  // check : is a floag wethear there is a skip (using the methon N*RQ.size()*quantum)
  int check = 0;

  // implement RQ and add Ps by its arraival time
  //ct : the curent time
  int64_t ct = 0, counter = 0, size = processes.size(), jobsRemaining = processes.size();

  int onCPU = -1; // Note processes ids start at 0, let -1 denote "idle"
  int64_t br = 0; // burstRemaining for ecah process
  // the ready queue
  std::vector < std::pair < int, int64_t >> RQ;

  while (1) {
    //===========================================================
    // condition #1 :: stop the simulation
    if (jobsRemaining == 0) break;
    //===========================================================
    // condition #2 :: a new process is arriving to RQ
    if (counter < size && (ct == processes[counter].arrival_time)){
        // add a new process :: start time of the process
      RQ.push_back(std::make_pair(processes[counter].id, processes[counter].burst));
      counter++;
      // if its the first process then add it to the seq.
      if(ct == 0 && seq.empty()) seq.push_back(0);
      continue;
    }
    //===========================================================
    //condition #3 :: if CPU is idle and RQ is not empty
    if ((onCPU == -1) && !(RQ.empty())) {
      // get info of the current Process
      onCPU = RQ.front().first;
      br = RQ.front().second;
      k = RQ.size(); // ***** get current size of RQ
      if(processes[onCPU].start_time==-1){
        processes[onCPU].start_time = ct; // 1
      }
      // ATF : -1 no comming proceses
      ATF = (counter < size) ? processes[counter].arrival_time : -1; // Y
      // This case for the last process, where we have one process left and there
      // is no coming proceses
      if (k == 1 && ATF == -1) {
        ct += br;

        RQ.erase(RQ.begin()); // erase the curent P
        processes[onCPU].finish_time = ct; // = current time
        jobsRemaining--;

        t = seq.size();
        if(seq.back()!= onCPU && t < max_seq_len) seq.push_back(onCPU);

        onCPU = -1;
        continue;
      }
      // set by default to 1
      check = 1;
      // if the current Ps in the RQ going to exceed the next coming process
      // then set check = 0, where k is RQ.size
      if (ATF != -1 && ((k * quantum) + ct > ATF)) check = 0;
      // If the following if stat. is FALSE then execute on p only from the RQ
      if (check && br - quantum > 0) {

        i = 0;
        // check all ps if would not finish if we at leas subtract a quantum amount from each one
        // on other word, If we are going to perform a BIG JUMP for all Ps in RQ then we need at least all of them
        // satisfy this condition
        while (i < k) {
            if (processes[RQ[i].first].start_time == -1) break;
            if (RQ[i].second - quantum > 0) i++;
            else break;
        }
        // if i = k that means we can perform a BIG JUMP
        if (i == k) {
          // we have 2 cases
          // case I : there is a coming process in the future
          // if this is the case then we should use it is arrival time as an upper limit
          // for the BIG JUMP
          if (ATF != -1){
                RV = skipCaseI(ATF, quantum, ct, RQ); // 2
                ct = (RV * k * quantum)+ct;
                j = RV * quantum;
                ITER = RV; // to fill the sequence
          }
          else
          {
                  i = 1;
                  j = br;
                  // CASE II : there is no coming jobs in the future
                  // get the lowest numbr of burestremain in the RQ
                  // use this number as our limit for the BIG JUMP
                  while(i<k){
                          if(RQ[i].second < j)
                                  j = RQ[i].second;
                          i++;
                  }
                  // divide to quantum to see how many skip or JUMPS we can do
                  j = j/quantum;
                  // if it 1 then leave otherewise
                  // decrement Becuase I do not wnat Ps to be finished here at this area
                  if(j != 1) j--;
                  ITER = j; // to fill the sequence
                  ct=(j*k*quantum)+ct;
                  j = j * quantum;
          }
          i = 0;
          // update number of bursts
          while (i < k) RQ[i++].second-= j;
          // *** fill sequence after the JUMP ***
          i = 0;
          t = seq.size();
          if(k == 1 && onCPU == seq.back()){
                onCPU = -1;
                continue;
          }else if(k == 1){
                seq.push_back(onCPU);
                onCPU = -1;
                continue;
          }
          while(ITER>0 && t < max_seq_len){
                i = 0;
                ITER--;
                t = seq.size();
                while(i < k && t < max_seq_len){
                        if(seq.back()!=RQ[i].first){
                                seq.push_back(RQ[i].first);
                                t++;
                        }
                        i++;
                }
          }
          onCPU = -1;
          continue;
        }
      }
      // execute on process only
      if (br - quantum > 0) {
        // current process is NOT going to be finished
        ct += quantum;
        br -= quantum;
        t = seq.size();
        if(seq.back()!= onCPU && t < max_seq_len) seq.push_back(onCPU);
      } else {
        // It going to be terminted after br of secondes
        ct += br;
        br = 0;
        jobsRemaining--;
        processes[onCPU].finish_time = ct;
        t = seq.size();
        if(seq.back()!= onCPU && t < max_seq_len) seq.push_back(onCPU);
      }
      if (ATF != -1 && ATF < ct) {
        // add coming Processes first : these proceses have arrive while skipping
        i = counter;
        while(i<size) {
          if (processes[i].arrival_time < ct){
            RQ.push_back(std::make_pair(processes[i].id, processes[i].burst));
            counter++;
          }
          else
            break;
          i++;
        }
      }
      // erase it and push_back
      RQ.erase(RQ.begin()); // erase the curent P
      if (br != 0) RQ.push_back(std::make_pair(onCPU, br));
      // set to idle
      onCPU = -1;
      continue;
    }
    //===========================================================
    // condition #4 :: if RQ is empty then JUMP to the next job
      if(RQ.empty()){
              t = seq.size();
              if(seq.back()!= onCPU && t<max_seq_len){
              seq.push_back(-1);
              }
              ct = processes[counter].arrival_time;
      }
      else ct++;
    }

  //===============================================
    // resize the seq()
    t = seq.size();
    if(t > max_seq_len)
        seq.resize(max_seq_len);
    else
       seq.resize(t);
}

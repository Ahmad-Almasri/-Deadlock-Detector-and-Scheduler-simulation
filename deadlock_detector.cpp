#include "deadlock_detector.h"
#include "common.h"
#include <iostream>
#include <vector>
#include <string>

class Graph {
        public:
        std::unordered_map<std::string, std::vector<std::string>> adj_list;
         std::unordered_map<std::string, int> out_counts;
}graph;

std::unordered_map<std::string, int> out;
std::vector<std::string> zero;

// return -1 if there is a cycle
int algo(Result &result){
    int found = 1; // if found return -1
    out = graph.out_counts; // a copy of out_counts
    zero.clear(); // reset the zero vector
    std::string n;
    for( auto &v : graph.out_counts)
           if(v.second == 0)
                  zero.push_back(v.first);
    // tarcking the array
    size_t counter = 0; // indecate of cycle has found
    while(zero.size() != 0){
        n = zero.back();
        zero.pop_back();
        counter++;
        // terate over the dj_list
        for(auto &n2 : graph.adj_list[n]){
                if(--out[n2]==0)
                        zero.push_back(n2);
        }
    }
    // if there is a cycle then stop
    if(counter != out.size()){
      std::string st = "p", st1;
      for(auto &v : out){
            st1 = v.first[0];
            if(v.second != 0 && st1==st){
              st1 = v.first;
              st1.erase(0,2);
              result.dl_procs.push_back(st1);
              found = -1;
            }
          }
        }
    return found;
}

Result detect_deadlock(const std::vector<std::string> & edges)
{
    Result result;
    result.edge_index = -1;
    std::vector<std::string> test;
    int temp = 0;
    int c = 0;
    for(auto & e : edges){
        test = split(e);
        // assign || request
        if(test[1] == "->"){
          // request
          // 0 is the outgoing
          graph.out_counts["p_"+test[0]]++;
          // 2 is the ingoing
          graph.adj_list["r_"+test[2]].push_back("p_"+test[0]);
          graph.out_counts["r_"+test[2]];
        }
        else{
          // assign
          // 0 is the outgoing
          graph.out_counts["r_"+test[2]]++;
          // 2 is the ingoing
          graph.adj_list["p_"+test[0]].push_back("r_"+test[2]);
          graph.out_counts["p_"+test[0]];
        }
        // after insert then call algo
        temp = algo(result);
        // stop the prog.
        if(temp == -1){
                result.edge_index=c;
                break;
        }
        c++;
  }
    return result;
}

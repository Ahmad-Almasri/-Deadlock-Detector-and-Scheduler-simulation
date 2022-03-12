/*

fcfsSimulationLoop.cpp

Example of FCFS simulation which follows the simulation loop pseudocode from lectures/tutorial. 

*/

#include <iostream>
#include <queue>
#include <utility>
#include <vector>

/**
 * Runs FCFS scheduling simulation
 * 
 * @param jobs : jobs to be scheduled. 
 *          Elements in job are pairs (arrival time, burst). 
 *          Jobs is assumed to be sorted based on arrival time. 
 *              
 */
void fcfsSimulation(std::vector<std::pair<int64_t, int64_t>> &jobs){

    int processOnCPU = -1; // Note processes ids start at 0, let -1 denote "idle"
    int64_t burstRemaining = 0;

    std::queue<std::pair<int, int64_t>> readyQueue; // elements are pairs: (process ID, burst remaining)
    int64_t currentTime = 0;
    int jobsArrived = 0;
    int jobsRemaining = jobs.size();

    // Execution order results
    std::vector<int> executionOrder;

    while(1){

        if (jobsRemaining == 0) break;
        
        // Check: if process on CPU is done
        if(processOnCPU > -1 && burstRemaining == 0)
        {
            processOnCPU = -1;
            jobsRemaining--;
            continue;
        }

        // Check: if a new process is arriving
        if (jobs.size() > 0 && jobs[jobsArrived].first == currentTime){
            readyQueue.push(std::make_pair(jobsArrived, jobs[jobsArrived].second));
            jobsArrived++;
            continue;
        }

        // Check: if CPU is idle and ready queue is not empty
        if (processOnCPU == -1 && !readyQueue.empty()){
            processOnCPU = readyQueue.front().first;
            burstRemaining = readyQueue.front().second;
            readyQueue.pop(); 
            continue;
        }

        // Update the execution order if needed
        if(executionOrder.empty() || executionOrder.back() != processOnCPU){
            executionOrder.push_back(processOnCPU);
        }

        // Print the current item on CPU
        if(processOnCPU >= 0) std::cout << "T" << currentTime << ":\t P" << processOnCPU << std::endl;
        else std::cout << "T" << currentTime << ":\t Idle" << std::endl;

        // Perform a CPU burst then increment the time
        if (burstRemaining > 0) burstRemaining--;
        currentTime++;
    }

    // Print the execution order
    bool printComma = false;
    std::cout << "Execution order: ";
    for(auto p : executionOrder){

        if (printComma) std::cout <<", ";
        else printComma = true;
        
        if(p >= 0) std::cout << "P" << p;
        else std::cout << "Idle";
    
    }
    std::cout << std::endl;
}

int main() {

    // Create some jobs / make some processes
    // Assume processes are sorted/added by arrival time
    std::vector<std::pair<int64_t, int64_t>> jobs; // elements are pairs: (arrival time, burst)
    jobs.push_back(std::make_pair(1, 5)); // process #0 arrives @ time 1, burst = 5
    jobs.push_back(std::make_pair(10, 4)); // process #1 arrives @ time 10, burst = 4
    jobs.push_back(std::make_pair(11, 3)); // process #2 arrives @ time 11, burst = 3

    // Run FCFS simulation
    fcfsSimulation(jobs);

    return 0;
}

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <cmath>

#include <array>
#include <stack>
#include <vector>
#include <unordered_map>
#include <tuple>

#include <cstdlib>
#include <ctime>
#include <climits>

#include "basicmatrix.h"
#include "basicsequence.h"

//find maximum B recursion function
int findMaxBRec(std::unordered_multimap<int,std::pair<int,int>> &SN,int start)
{
  int b = 1,temp;
  auto its = SN.equal_range(start);
  for (auto it = its.first; it != its.second; ++it)
    if(it->first != it->second.first){
      temp = findMaxBRec(SN,it->second.first);
      b += temp;
      it->second.second = temp;//traffic flow
    }
  return b;
}

//calculate the <Wn>
double calWRec(std::unordered_multimap<int,std::pair<int,int>> &SN,int start,int L,int total_trafic)
{
  int total_traffic2;
  double w = 0;
  auto its = SN.equal_range(start);
  for (auto it = its.first; it != its.second; ++it)
    if(it->first != it->second.first){
      total_traffic2 = total_trafic + it->second.second; //total traffic flow from it->second.second to the attractor
      w += (double)total_traffic2/L + calWRec(SN,it->second.first,L+1,total_traffic2);
    }
  return w;
}

//find the maximum B:basin for a given directed network
std::pair<int,double> findMaxBW(std::unordered_multimap<int,std::pair<int,int>> &SN)
{
  int maxB = 0,temp,attractor = 0;
  double Wn;
  for(auto it = SN.begin(); it != SN.end(); it++){
    if(it->first == it->second.first && it->first == 272){
      temp = findMaxBRec(SN,it->second.first);
      if(temp > maxB){
	maxB = temp;
	attractor = it->first;//main attractor
      }
    }
  }
  Wn = calWRec(SN,attractor,1,0);
  return std::make_pair(maxB,Wn/maxB);
}

//calculate the next end state 
void nextEndState(int *network,Sequence& start_state,Sequence& end_state,int size)
{
  int temp;
  /****basical model*****/
  // for(int i = 0;i < size;i++){//calculate S(i+1)
  //     temp = 0;     //result
  //     for(int j = 0;j < size;j++)
  //   	if(start_state[j]) temp += network[i*size+j];
  //     end_state[i] = temp<=0?false:true;
  // }

  /****forbid domination model****/
  for(int i = 0;i < size;i++){//for every onde i
    temp = 0;     //result
    for(int j = 0;j < size;j++) //for all j
      if(start_state[j]){  //j is active
	if(network[i*size+j] == -1){
	  if(j != i) {
	    temp = -100; //strong inhibitory,special case
	    break;
	  }
	}
      	temp += network[i*size+j];
      }

    if(temp < 0)
      end_state[i] = 0;
    else if(temp == 0)
      end_state[i] = start_state[i];
    else
      end_state[i] = true;
  }
}

int* f(int size,int n){
  int *A = new int [size];
  for(int i =0;i<size;i++){
    A[i] = 0;
  }
  return A;
}

//give a network,and generate its B:basin,W:the overlap of all trajectory 
std::pair<int,double> genBW(int *network,int size)
{
  bool set[2] = {false,true}; //it is useless
  Sequence start_state(size,2);
  Sequence end_state(size,2);

  std::unordered_multimap<int,std::pair<int,int>> stateNetwork;
  int stateSum = pow(2,size);
  for(int i = 0;i < size;i++)
    start_state[i] = true;
  for(int stateNum = 0;stateNum < stateSum;stateNum++){
    //calculate the end state
    nextEndState(network,start_state,end_state,size);
    //store it to a map:start_state->(end_state,b)
    stateNetwork.insert(std::unordered_multimap<int,std::pair<int,int>>::value_type 
			((int)end_state,std::make_pair((int)start_state,0)));
    //for every starting state,get the next state
    start_state.nextSequence();
  }  
  return findMaxBW(stateNetwork);
}


//write all possible network b-w to a file
void allPossibleNetworkBW(char *fullNet,char *coreNet,int nodeNum)
{
  int *m = genMatrix(fullNet,nodeNum,nodeNum);
  auto bw = genBW(m,nodeNum);
  std::cout<<bw.first<<" "<<bw.second<<std::endl;
  //prtMatrix(m,nodeNum,nodeNum);
  int *m2 = new int[nodeNum*nodeNum];

  //enumerate all rigid edge
  // int rigidEdgeStart[10] = {1,1,4,6,7,4,7,8,8,11};
  // int rigidEdgeEnd[10] = {2,3,5,6,6,9,10,10,11,11};
  // Sequence rigidSeq(10,2);
  // rigidSeq.nextSequence(); //exclude the 00000000 case
  // int caseNum = pow(2,10);
  // for(int i = 0; i < caseNum-1; ++i){
  //   copyMatrix(m,m2,nodeNum,nodeNum);
  //   for(int j = 0; j < 10; j++)
  //     if(rigidSeq[j] == 1)
  // 	m2[rigidEdgeStart[j] + nodeNum*(rigidEdgeEnd[j]-1) - 1] = 0;
  //   bw = genBW(m2,nodeNum);
  //   std::cout<<bw.first<<" "<<bw.second<<std::endl;
  //   rigidSeq.nextSequence();
  // }

  //enumerate all supplement edge
  // int supplementalEdgeStart[11] = {5,6,8,8,9,10,10,10,10,11,11};
  // int supplementalEdgeEnd[11] = {10,9,9,5,10,5,7,9,11,10,6};
  // Sequence supplementalSeq(11,2);
  // supplementalSeq.nextSequence(); //exclude the 00000000 case
  // int caseNum = pow(2,11);
  // for(int i = 0; i < caseNum-1; ++i){
  //   copyMatrix(m,m2,nodeNum,nodeNum);
  //   for(int j = 0; j < 11; j++)
  //     if(supplementalSeq[j] == 1)
  // 	m2[supplementalEdgeStart[j] + nodeNum*(supplementalEdgeEnd[j]-1) - 1] = 0;
  //   bw = genBW(m2,nodeNum);
  //   std::cout<<bw.first<<" "<<bw.second<<std::endl;
  //   supplementalSeq.nextSequence();
  // }

  //enumerate all interchangeable edges
  // int interchangeableEdgeStart[13] = {1,10,10,4,3,7,10,7,11,2,7,9,7};
  // int interchangeableEdgeEnd[13] = {1,2,3,4,4,5,6,7,7,8,8,8,9};
  // Sequence interchangeableSeq(13,2);
  // interchangeableSeq.nextSequence(); //exclude the 00000000 case
  // int caseNum = pow(2,13);
  // for(int i = 0; i < caseNum-1; ++i){
  //   copyMatrix(m,m2,nodeNum,nodeNum);
  //   for(int j = 0; j < 13; j++)
  //     if(interchangeableSeq[j] == 1)
  // 	m2[interchangeableEdgeStart[j] + nodeNum*(interchangeableEdgeEnd[j]-1) - 1] = 0;
  //   bw = genBW(m2,nodeNum);
  //   std::cout<<bw.first<<" "<<bw.second<<std::endl;
  //   interchangeableSeq.nextSequence();
  // }

}

int main(int argc,char * argv[])
{
  if (argc != 4) //quit if not input "filename" "size"
  {
    std::cerr<<"Usage: "<<argv[0]<<" fullnetwork "<<" corenetwork  "<<" nodeSize "<<std::endl;
    exit(EXIT_FAILURE);
  }
  int nodeNum = atoi(argv[3]);
  std::clock_t c_start = std::clock();
  
  //int network[16] = {-1,1,1,-1,1,1,-1,0,0,0,0,1,1,-1,1,0};
  int network[121] = {-1,0,0,0,0,0,0,0,0,0,0,
  		      1,0,0,0,0,0,0,0,0,-1,0,
  		      1,0,0,0,0,0,0,0,0,-1,0,
  		      0,0,1,-1,0,0,0,0,0,0,0,
  		      0,0,0,-1,0,0,1,0,0,0,0,
  		      0,0,0,0,0,-1,1,0,0,-1,0,
  		      0,0,0,0,0,0,-1,0,0,0,1,
  		      0,1,0,0,0,0,-1,0,-1,0,0,
  		      0,0,0,-1,0,0,1,0,0,0,0,
  		      0,0,0,0,0,0,-1,1,0,0,0,
  		      0,0,0,0,0,0,0,1,0,0,-1};
		     

  auto bw = genBW(network,11);
  std::cout<<"B = "<<bw.first<<" W = "<<bw.second<<std::endl;
  allPossibleNetworkBW(argv[1],argv[2],nodeNum);
  std::clock_t c_end = std::clock();
  std::cout<<"It costs : "<<(c_end-c_start)<<" ms."<<std::endl;
}

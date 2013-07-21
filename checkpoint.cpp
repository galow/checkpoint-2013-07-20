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


//calculate the next end state 
void nextEndState(int *network,Sequence& start_state,Sequence& end_state,int size)
{
  int temp;

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
    //!!!!! add && network[i*size+i] != 1 condition !!!!!!
    else if(temp == 0 && network[i*size+i] != 1)
      end_state[i] = start_state[i];
    else
      end_state[i] = true;
  }

   /*********boolean model********/
  // for(int i = 0;i < size;i++){//for every onde i
  //   temp = 0;     //result
  //   for(int j = 0;j < size;j++) //for all j
  //     if(start_state[j])
  //     	temp += (network[i*size+j]==-1)?-1:network[i*size+j]; //b = (a==0)?1:2

  //   if(temp < 0)
  //     end_state[i] = 0;
  //   else if(temp == 0)
  //     end_state[i] = start_state[i];
  //   else
  //     end_state[i] = true;
  // }
}


//find maximum B recursion function
int findMaxBRec(std::unordered_multimap<int,std::pair<int,int>> &SN,int start,double *TF)
{
  int b = 1,temp;
  //int b = 0,temp;
  auto its = SN.equal_range(start);
  for (auto it = its.first; it != its.second; ++it)
    if(it->first != it->second.first){
      temp = findMaxBRec(SN,it->second.first,TF);
      b += temp;
      //findMaxBRec(SN,it->second.first,TF);
      //b += 1;
      it->second.second = temp;//traffic flow
    }
  TF[start] = b;
  return b;
}

//calculate the <Wn>,and store the W for each attractor
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
  //store the traffic flow!!!!this is not traffic flow!!!!!
  //TF[start] = w;
  return w;
}

//find the maximum B:basin for a given directed network
std::pair<int,double> findMaxBW(std::unordered_multimap<int,std::pair<int,int>> &SN,double *TF)
{
  int maxB = 0,temp,attractor = 0;
  double Wn;
  for(auto it = SN.begin(); it != SN.end(); it++){
    /**!!!!!!!!!!!your must change attractor!!!!!!!!**/
    if(it->first == it->second.first && it->first == 76){
      temp = findMaxBRec(SN,it->second.first,TF);
      if(temp > maxB){
	maxB = temp;
	attractor = it->first;//main attractor
      }
    }
  }
  Wn = calWRec(SN,attractor,1,0);
  return std::make_pair(maxB,Wn/maxB);
}


//give a network,and generate its B:basin,W:the overlap of all trajectory 
void genBW(int *network,int size)
{
  bool set[2] = {false,true}; //it is useless
  Sequence start_state(size,2);
  Sequence end_state(size,2);

  std::unordered_multimap<int,std::pair<int,int>> stateNetwork;
  int stateSum = pow(2,size);
  double *TF = new double [stateSum];

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

  auto bw = findMaxBW(stateNetwork,TF);
  
  //statistics the checkpoint
  double *weight1 = new double[size];
  double *weight2 = new double[size];
  double *weight = new double[size];

  int *count1 = new int[size];
  int *count2 = new int[size];
  int *count = new int[size];
  for(int i = 0; i < size; ++i){
    count1[i] = 0;
    count2[i] = 0;
    count[i] = 0;
  }
  int state;
  start_state.reset();
  for(int i = 0; i < stateSum;i++){
    state = (int)start_state;
    for(int j = 0; j < size; j++)
      std::cout<<start_state[j]<<" ";
    std::cout<<TF[state]<<std::endl;
    if(TF[state] >= 1){
      for(int j = 0; j < size; j++){
	if(start_state[j] == 1){
	  weight1[j] += TF[state];
	  count1[j] += 1;
	}
	else{
	  weight2[j] -= TF[state];
	  count2[j] += 1;
	}
	
	weight[j] += TF[state];
	count[j] += 1;
      }
    }
    start_state.nextSequence();
  }
  std::cout<<"B = "<<bw.first<<" W = "<<bw.second<<std::endl;
  for(int j = 0; j < size; j++)
    std::cout<<(j+1)<<"  "<<count1[j]<<" "<<count2[j]<<" "<<
      weight1[j]<<" "<<weight2[j]<<" "<<(weight1[j]+weight2[j])/weight[j]<<std::endl;

  delete weight1;
  delete weight2;
  delete weight;
  delete count1;
  delete count2;
  delete count;
  delete TF;
}


int main(int argc,char * argv[])
{
  if (argc != 3) //quit if not input "filename" "size"
  {
    std::cerr<<"Usage: "<<argv[0]<<" fullnetwork "<<" nodeSize "<<std::endl;
    exit(EXIT_FAILURE);
  }
  int nodeNum = atoi(argv[2]);
  std::clock_t c_start = std::clock();
  
  int *network = genMatrix(argv[1],nodeNum,nodeNum);

  genBW(network,nodeNum);

  std::clock_t c_end = std::clock();
  std::cout<<"It costs : "<<(c_end-c_start)<<" ms."<<std::endl;
}

/*****this head file define some basic matrix operation
****author YB.Ma
****date 2013.3.29
****version 1.0
*****/
#ifndef BASICMATRIX_H_
#define BASICMATRIX_H_

#include<iostream>
#include <fstream>
#include <iomanip>

//generate a integer matrix from file
int * genMatrix(char * fileName,int row,int col)
{
  int * matrix = new int [row*col];
  std::ifstream inFile;
  inFile.open(fileName);
  if(!inFile.is_open()) //fail to open a file
  {
    std::cout<<"Could not open a file:"<<fileName<<std::endl;
    exit(EXIT_FAILURE);
  }
  int temp;
  for(int r = 0;r < row;r++)
  {
    for(int c = 0;c < col;c++)
    {
      inFile>>temp;
      matrix[r*col+c] = temp;
    }
  }
  inFile.close();
  return matrix;
}

//delete matrix
void delMatrix(int *M){delete [] M;}

//print a integer matrix
void prtMatrix(int *M,int row,int col)
{
  for(int r = 0;r < row;r++){
    for(int c = 0;c < col;c++)
      std::cout<<std::setw(4)<<M[r*col+c];
    std::cout<<std::endl;
  }
}

//copy matrix
void copyMatrix(int *from,int *to,int row,int col){
  int N = row * col;
  for(int i = 0;i < N;i++)
    to[i] = from[i];
}

#endif

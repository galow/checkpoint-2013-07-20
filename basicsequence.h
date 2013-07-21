/****this head file define a sequence class,a sequence is like this:1001000100
example:Sequence seq(8,2) will create a sequence with length 8 and every opsiiton can be 0 or 1.
this a 2 bit sequence with length 10
@author YB.Ma
@date 2013.3.29
@version 1.0
*****/

#ifndef BASICSEQUENCE_H_
#define BASICSEQUENCE_H_

#include <iostream>

class Sequence{
protected:
  int sequenceLength;
  int *sequence;//every element store a num from 0->setSize-1
  int setSize; //candidate's set size
  
  //set the s to next candidate,if it has carry return true
  virtual bool nextCand(int & s);
public:
  Sequence(); //default constructor
  Sequence(int sl,int cl);
  Sequence(const Sequence &seq);//copy constructor
  Sequence & operator=(const Sequence &);//copy operator
  ~Sequence(){delete sequence;}
  void reset();
  //change to next sequence
  void nextSequence();
  //cast sequence to integer
  operator int () const;
  friend std::ostream& operator << (std::ostream& os,const Sequence& seq){
    for(int i = 0;i < seq.sequenceLength;i++)
      os<<std::setw(4)<<seq[i];
    return os;
  }
  int &operator [](int i) const {/*****your can modify sequence*****/
    if(i > sequenceLength-1){
      std::cerr<<"Sequence [] operator out of range."<<std::endl;
      i = 0;
    }
    return sequence[i];
  }
};

Sequence::Sequence(){ //a 0 sequence
  sequenceLength = 1;
  setSize = 1;
  sequence = new int [sequenceLength];
  sequence[0] = 0;
}

Sequence::Sequence(int sl,int ss){
  sequenceLength = sl;
  setSize = ss;
  //create the sequence
  sequence = new int [sequenceLength];
  //initialize the quence
  for(int i = 0;i < sequenceLength;i++)
    sequence[i] = 0;
}

Sequence::Sequence(const Sequence & seq){
  sequenceLength = seq.sequenceLength;
  sequence = new int [sequenceLength];
  for(int i = 0;i < sequenceLength;i++)
    sequence[i] = seq.sequence[i];
  setSize = seq.setSize;
}

Sequence & Sequence::operator=(const Sequence & seq){
  if(this != &seq){
    sequenceLength = seq.sequenceLength;
    delete sequence;
    sequence = new int [sequenceLength];
    for(int i = 0;i < sequenceLength;i++)
      sequence[i] = seq.sequence[i];
    setSize = seq.setSize;
  }
  return *this;
}

bool Sequence::nextCand(int & s){
  if(s < setSize - 1){
    s++;
    return false;
  }
  s = 0;
  return true;
}


void Sequence::reset(){
  for(int i = 0;i < sequenceLength;i++)
    sequence[i] = 0;
}

Sequence::operator int () const{
  int integer = 0;
  for(int i = sequenceLength-1;i > 0;i--)
    integer = (integer+sequence[i])*setSize; //can shift << 1
  integer += sequence[0];
  return integer;
}

void Sequence::nextSequence(){
  int index = 0,carry;
  do carry = nextCand(sequence[index]);
  while(carry && ++index < sequenceLength);
}

#endif

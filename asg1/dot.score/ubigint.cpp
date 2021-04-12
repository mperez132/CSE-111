// $Id: ubigint.cpp,v 1.12 2020-10-19 13:14:59-07 - - $
//-- Constantine Kolokousis (kkolokou@ucsc.edu)
//--Moises Perez(mperez86@ucsc.edu)
#include <cctype>
#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
#include <cmath>
using namespace std;

#include "debug.h"
#include "relops.h"
#include "ubigint.h"

ubigint::ubigint (unsigned long that) {

   DEBUGF('~', this << " -> " << *this);
   if (that == 0) {
      ubig_value.push_back(that);
   }
   else {
      ubig_value.push_back(that);
      //We need to remove the zeros from the number.
      while (ubig_value.size() > 0 and ubig_value.back() == 0) {
         ubig_value.pop_back();
      }
   }
}

ubigint::ubigint (const string& that) {

   DEBUGF ('~', "that = \"" << that << "\"");
   for (char digit: that) {
      if (not isdigit (digit)) {
         throw invalid_argument ("ubigint::ubigint(" + that + ")");
      }
      ubig_value.insert((ubig_value.begin()), (digit - '0'));
   }
   //We need to remove the zeros from the number.
   while (ubig_value.size() > 0 and ubig_value.back() == 0) {
      ubig_value.pop_back();
   }
}

//To implement ubigint::operator+
//create a new ubigint and proceed from the
//low order end to the high order end, adding digits pairwise.
//For any sum >= 10, take the remainder and add the 
//carry to the next digit. Use push_back to append the new digits to 
//the ubigint. When you run out of digits in the
//shorter number, continue, matching the longer vector with zeros
//until it is done. Make sure the sign of 0 is positive.

ubigint ubigint::operator+ (const ubigint& that) const {

   DEBUGF ('u', *this << "+" << that);
   ubigint result;
   bool CarryFlag = 0;
   int add;
   DEBUGF ('u', result);

   //Need to get the beginning of each of the ubig_value
   auto left_index = ubig_value.begin();
   auto right_index = that.ubig_value.begin();

   for (; left_index != ubig_value.end() &&
      right_index != that.ubig_value.end();
      left_index++, right_index++) {

      add = (*left_index + *right_index);
      if (CarryFlag == 1) {
         add += 1;
      }
      if (add >= 10) {
         add -= 10;
         CarryFlag = 1;
      }
      else {
         CarryFlag = 0;
      }
      result.ubig_value.push_back(add);
   }
   //One is not near the end
   for (; left_index != ubig_value.cend(); left_index++) {
      
      add = (*left_index);
      if (CarryFlag == 1) {
         add += 1;
      }
      if (add >= 10) {
         add -= 10;
         CarryFlag = 1;
      }
      else {
         CarryFlag = 0;
      }
      result.ubig_value.push_back(add);
   }
   //Other is not near the end
   for (; right_index != that.ubig_value.cend(); right_index++) {
      
      add = (*right_index);
      if (CarryFlag == 1) {
         add += 1;
      }
      if (add >= 10) {
         add -= 10;
         CarryFlag = 1;
      }
      else {
         CarryFlag = 0;
      }
      result.ubig_value.push_back(add);
   }
   //end with carry
   if (CarryFlag == 1) {
      result.ubig_value.push_back(1);
   }
   //We need to remove the zeros from the number.
   while (result.ubig_value.size() > 0 and 
      result.ubig_value.back() == 0) {
      result.ubig_value.pop_back();
   }
   return result;
}

//To implement ubigint::operator-, also create a new empty vector, 
//starting from the low order endand continuing until the high end. 
//If the left digit is smaller than the right digit, 
//the subtraction will be less than zero.
//In thatcase, add 10 to the digit
//and set the borrow to the next digit to -1. After the
//algorithm is done, pop_back all high order zeros 
//from the vector before returning it. 
//Make sure the sign of 0 is positive.

ubigint ubigint::operator- (const ubigint& that) const {

   if (*this < that) throw domain_error("ubigint::operator-(a<b)");
   ubigint result;
   bool BorrowFlag = 0;
   int sub = 0;
   //Need to get the beginning of each of the ubig_value
   auto left_index = ubig_value.begin();
   auto right_index = that.ubig_value.begin();

   for (; left_index != ubig_value.end() &&
      right_index != that.ubig_value.end();
      left_index++, right_index++) {

      sub = (*left_index - *right_index);
      if (BorrowFlag == 1) {
         sub -= 1;
         BorrowFlag = 0;
      }

      if (sub < 0) {
         sub += 10;
         BorrowFlag = 1;
      }
      result.ubig_value.push_back(sub);
   }
   //One is not near the end
   for (; left_index != ubig_value.cend(); left_index++) {
      
      sub = (*left_index);
      if (BorrowFlag == 1) {
         sub -= 1;
      }
      if (sub < 0) {
         sub += 10;
         BorrowFlag = 1;
      }
      else {
         BorrowFlag = 0;
      }
      result.ubig_value.push_back(sub);
   }
   //Other is not near the end
   for (; right_index != that.ubig_value.cend(); right_index++) {
      
      sub = (*right_index);
      if (BorrowFlag == 1) {
         sub -= 1;
      }
      if (sub < 0) {
         sub += 10;
         BorrowFlag = 1;
      }
      else {
         BorrowFlag = 0;
      }
      result.ubig_value.push_back(sub);
   }
   //We need to remove the zeros from the number.
   while (result.ubig_value.size() > 0 and
      result.ubig_value.back() == 0) {
      result.ubig_value.pop_back();
   }
   return result;
}

//Multiplication in ubigint::operator* 
//proceeds by allocating a new vector whose size is equal to the sum 
//of the sizes of the other two operands. If u is a vector of size m 
//and v is a vector of size n, then in O(mn) speed, perform an
//outer loop over one argument and an inner loop over 
//the other argument, adding the new partial products to 
//the product p as you would by hand.

//Given from algorithm on asg1 pdf
//Source for floor :
//https://www.programiz.com/cpp-programming/library-function/cmath/floor

ubigint ubigint::operator* (const ubigint& that) const {
   
   //Given
   ubigint result;
   int m = ubig_value.size();
   int n = that.ubig_value.size();
   int d = 0;
   result.ubig_value.resize(m + n);

   for (int i = 0; i < m; i++) {
      int c = 0;
      d = 0;
      for (int j = 0; j < n; j++) {
         d = result.ubig_value.at(i + j)
            + ubig_value.at(i)*that.ubig_value.at(j)
               + c;
         result.ubig_value.at(i + j) = d % 10;
         c = floor(d / 10);
      }
      result.ubig_value.at(i + n) = c;
   }
   while (result.ubig_value.size() > 0 and
      result.ubig_value.back() == 0) {
      result.ubig_value.pop_back();
   }
   return result;
}
//Need to multiply by two
void ubigint::multiply_by_2() {

   ubigint result;
   result.ubig_value = ubig_value;
   result = result * 2;

   while (result.ubig_value.size() > 0 and
      result.ubig_value.back() == 0) {
      result.ubig_value.pop_back();
   }
   ubig_value = result.ubig_value;
}
//Works now
void ubigint::divide_by_2() {

   ubigint result;
   result.ubig_value = ubig_value;
   //result = result / 2;

   int m = result.ubig_value.size();
   int i = 0;
   while(i < m) {
      result.ubig_value[i] = result.ubig_value[i] / 2;
      if (i < m - 1 && result.ubig_value[i + 1] % 2 != 0) {
         result.ubig_value[i] = result.ubig_value[i] + 5;
      }
      i++;
   }
   while (result.ubig_value.size() > 0 and
      result.ubig_value.back() == 0) {
      result.ubig_value.pop_back();
   }
   ubig_value = result.ubig_value;
}

struct quo_rem { ubigint quotient; ubigint remainder; };
quo_rem udivide (const ubigint& dividend, const ubigint& divisor_) {
   // NOTE: udivide is a non-member function.
   ubigint divisor {divisor_};
   ubigint zero {0};
   if (divisor == zero) throw domain_error ("udivide by zero");
   ubigint power_of_2 {1};
   ubigint quotient {0};
   ubigint remainder {dividend}; // left operand, dividend
   while (divisor < remainder) {
      divisor.multiply_by_2();
      power_of_2.multiply_by_2();
   }
   while (power_of_2 > zero) {
      if (divisor <= remainder) {
         remainder = remainder - divisor;
         quotient = quotient + power_of_2;
      }
      divisor.divide_by_2();
      power_of_2.divide_by_2();
   }
   DEBUGF ('/', "quotient = " << quotient);
   DEBUGF ('/', "remainder = " << remainder);
   return {.quotient = quotient, .remainder = remainder};
}

ubigint ubigint::operator/ (const ubigint& that) const {
   return udivide (*this, that).quotient;
}

ubigint ubigint::operator% (const ubigint& that) const {
   return udivide (*this, that).remainder;
}

//To implement operator==, 
//check to see if the signs are the same and 
//the lengths of the vectors are the same.If not, return false.
//Otherwise run down both vectors and return false 
//as soon a difference is found. Otherwise return true.

bool ubigint::operator== (const ubigint& that) const {
   //We can just check the size of the numbers
   if (ubig_value.size() != that.ubig_value.size()) {
      return false;
   }
   else {
      //Go through each and compare
      //If at any point does it not equal it will return false.
      for (long unsigned int i = 0; i < that.ubig_value.size(); i++) {
         if (ubig_value[i] != that.ubig_value[i]) {
            return false;
         }
      }
      return true;
   }
}

//To implement operator<, 
//remember that a negative number is less than a positive number.
//If the signs are the same, for positive numbers, the shorter one is
//less, and for negative nubmers, the longer one is less.
//If the signsand lengths are the same, run down the parallel 
//vectors from the high order end to the low order end. 
//When a difference is found, return true or false, as appropriate.If
//no difference is found, return false.

bool ubigint::operator< (const ubigint& that) const {
   
   //compare sizes
   if (ubig_value.size() < that.ubig_value.size()) {
      return true;
   }
   else if (ubig_value.size() > that.ubig_value.size()) {
      return false;
   }
   else {
      for (int i = ubig_value.size() - 1; i >= 0; i--) {
         if (ubig_value[i] > that.ubig_value[i]) {
            return false;
         }
         else if (ubig_value[i] < that.ubig_value[i]) {
            return true;
         }
      }
   }
   return false;
}

ostream& operator<< (ostream& out, const ubigint& that) { 

   if (that.ubig_value.size() > 0) {
      int count = 0;
      int beg = that.ubig_value.size() - 1;
      while (beg >= 0) {
         count++;
         out << static_cast<unsigned>(that.ubig_value[beg]);

         if (count % 69 == 0 && count != 0) {
            out << "\\\n";
         }
         beg--;
      }
   }
   else if(that.ubig_value.size() == 0){
      return out << "0";
   }
   else {
      return out;
   }
   return out;
}


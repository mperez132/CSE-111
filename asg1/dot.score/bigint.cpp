// $Id: bigint.cpp,v 1.3 2020-10-11 12:47:51-07 - - $
//-- Constantine Kolokousis (kkolokou@ucsc.edu)
//--Moises Perez(mperez86@ucsc.edu)
#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;

#include "bigint.h"

bigint::bigint (long that): uvalue (that), is_negative (that < 0) {
   DEBUGF ('~', this << " -> " << uvalue)
}

bigint::bigint (const ubigint& uvalue_, bool is_negative_):
                uvalue(uvalue_), is_negative(is_negative_) {
}

bigint::bigint (const string& that) {
   is_negative = that.size() > 0 and that[0] == '_';
   uvalue = ubigint (that.substr (is_negative ? 1 : 0));
}

bigint bigint::operator+ () const {
   return *this;
}

bigint bigint::operator- () const {
   return {uvalue, not is_negative};
}

//Work here
bigint bigint::operator+ (const bigint& that) const {
        
   bigint result;
   //If they're the same sign
   if (is_negative == that.is_negative) {
       result.uvalue = uvalue + that.uvalue;
       result.is_negative = that.is_negative;
   }
   //Now we know we need to subtract. 
   //Therefore compare both numbers to see which to subtract
   else {
       if (uvalue > that.uvalue) {
           result.uvalue = uvalue - that.uvalue;
           result.is_negative = is_negative;
       }
       else if (uvalue < that.uvalue) {
           result.uvalue = that.uvalue - uvalue;
           result.is_negative = that.is_negative;
       }
       else {
           result.uvalue = uvalue - that.uvalue;
           result.is_negative = false;
       }
   }
   return result;
}
//Work here alsmost same as operator+ just opposite~ish
bigint bigint::operator- (const bigint& that) const {
   bigint result;
   //If they're the same sign
   //and since it's subtracting we need to check which one is greater
   if (is_negative == that.is_negative) {

       if (uvalue < that.uvalue) {
           result.uvalue = that.uvalue - uvalue;
           result.is_negative = !(that.is_negative);

       }
       else if (uvalue > that.uvalue) {
           result.uvalue = uvalue - that.uvalue;
           result.is_negative = is_negative;
       }
       else {
          result.uvalue = uvalue - that.uvalue;
          result.is_negative = false;
       }
   }
   //Now we know they are different signs
   else {
       result.uvalue = uvalue + that.uvalue;
       result.is_negative = is_negative;
   }
   return result;
}

//Work here
bigint bigint::operator* (const bigint& that) const {

   bigint result;
   result.uvalue = uvalue * that.uvalue;
   //Same sign
   if ((is_negative == that.is_negative) || 
      (not is_negative and not that.is_negative)) {
       result.is_negative = false;
   }
   else {
       result.is_negative = true;
   }
   return result;
}
//Work here
bigint bigint::operator/ (const bigint& that) const {

   bigint result;
   result.uvalue = uvalue / that.uvalue;

   if ((is_negative != that.is_negative)) {
       result.is_negative = true;
   }
   else {
       result.is_negative = false;
   }
   return result;
}
//Work here
bigint bigint::operator% (const bigint& that) const {

   bigint result;
   result.uvalue = uvalue % that.uvalue;
   result.is_negative = is_negative;
   return result;
}

bool bigint::operator== (const bigint& that) const {
   return is_negative == that.is_negative and uvalue == that.uvalue;
}

bool bigint::operator< (const bigint& that) const {
   if (is_negative != that.is_negative) return is_negative;
   return is_negative ? uvalue > that.uvalue
                      : uvalue < that.uvalue;
}
//Work here
ostream& operator<< (ostream& out, const bigint& that) {
    return out << (that.is_negative ? "-" : "")
        << that.uvalue;
}


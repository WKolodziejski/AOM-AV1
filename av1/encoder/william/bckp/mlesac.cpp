#include "mlesac.h"

#include <iostream>
using namespace std;

//#ifdef __cplusplus
//extern "C" {
//#endif

class mlesac {
 public:
  void print(char *str) {
    cout << str;
  }
};

void *mlesac_create() {
  return new mlesac;
}

void mlesac_release(void *mle) {
  delete static_cast<mlesac *>(mle);
}

void mlesac_print(void *mle, char *str) {
  static_cast<mlesac *>(mle)->print(str);
}

//#ifdef __cplusplus
//}
//#endif
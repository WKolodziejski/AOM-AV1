#ifndef AOM_MLESAC_H
#define AOM_MLESAC_H

#ifdef __cplusplus
extern "C" {
#endif

void *mlesac_create();
void mlesac_release(void *mle);
void mlesac_print(void *mle, char *str);

#ifdef __cplusplus
}
#endif

#endif  // AOM_MLESAC_H

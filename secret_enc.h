#ifndef __SECRET_ENC_H__
#define __SECRET_ENC_H__


struct secret
{
    unsigned int size;
    char* data;
};

struct secret* copy_secret (const char*);

#endif /* __SECRET_ENC_H__ */

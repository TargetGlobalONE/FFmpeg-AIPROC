#ifndef __SECRET_ENC_H__
#define __SECRET_ENC_H__
#include <stdlib.h>
#include <stdio.h>

struct secret
{
  unsigned int size;
  char* data;
};

struct secret* copy_secret (const char*);

struct secret* copy_secret (const char *name)
{
    FILE *file;
    unsigned int file_len;
    struct secret* ret = NULL;

    //open file
    file = fopen (name, "rb");
    if (!file)
    {
	av_log (NULL, AV_LOG_ERROR, "Unable to open file %s", name);
	return NULL;
    }
	
    //Get file length
    fseek (file, 0, SEEK_END);
    file_len = ftell (file);
    fseek (file, 0, SEEK_SET);

    //Allocate memory
    ret = (struct secret *) av_malloc (sizeof (struct secret));
    ret->data = (char*) av_malloc (file_len + 1);
    if (!ret)
    {
	av_log (NULL, AV_LOG_ERROR, "Memory error!");
        fclose (file);
	return NULL;
    }

    //Read file contents into buffer
    fread (ret->data, file_len, 1, file);
    ret->size = file_len;
    fclose (file);

    return ret;
}

#endif /* __SECRET_ENC_H__ */

#include "ig_base.h"
#include <stdlib.h>

//For TEXT files since it will null terminate.
static char *
ReadEntireFileIntoMemoryAndNullTerminate(char *FileName)
{
    char *Result = 0;
    
    FILE *File = fopen(FileName, "r");
    if(File)
    {
		//Kill UTF Header
		size_t pos = 0;
		int ch = 0;
		while (File)
		{
			ch = fgetc(File);
			if (ch == 0xef ||
				ch == 0xbb ||
				ch == 0xbf)
			{
				++pos;
			}
			else
			{
				break;
			}
		}
		
		fseek(File, 0, SEEK_END);
		size_t FileSize = ftell(File);
        fseek(File, pos, SEEK_SET);

		size_t allocSize = FileSize + 1 - pos;
		if (allocSize > 0)
		{
			Result = (char *)malloc(allocSize);
			fread(Result, FileSize - pos, 1, File);
			Result[FileSize] = 0;
		}
        
        
        fclose(File);
    }
    return(Result);
}

static void
copyStringLiteral(char *dst, char *src, int len)
{
    for (int i = 0; i < len; ++i)
    {
        *dst++ = *src++;
    }
    *dst = 0; // null terminate
}

/*
static size_t
writeCSVMessages(can_message* message_root, char* FileName)
{
    size_t Result = 0;
    
    FILE *File = fopen(FileName, "w");
    if(File)
    {
        can_message *message = message_root;
        while (message)
        {
            fprintf(File, "%d,%f,%u,%08X,%d,", message->id, message->timestamp, 
                                                (bool)message->Rx, *((unsigned int*)(&message->header)), 
                                                message->datalength);
            for (int i = 0; 
                i < message->datalength; 
                i++)
            {
                fprintf(File, "%02X", (unsigned char)message->data[i]);
            }
            fprintf(File, "\n");
            message = message->next;
        }
        
        Result = ftell(File);
        fclose(File);
    }
    return(Result);
}

static size_t
writeMessages(can_message* message_root, char* FileName)
{
	size_t Result = 0;
	u32 message_count = 0;
    
    FILE *File = fopen(FileName, "wb");
    if(File)
    {
        can_message *message = message_root;
        while (message)
        {
        	//the size varies.  would also have to fix up pointers.
        	fwrite(message, sizeof(can_message),1,File);
        	fwrite(message->data, sizeof(u8), message->datalength, File);
        	message = message->next;
        	++message_count;
        }
        Result = ftell(File);
        printf("Wrote %u messages\n", message_count);
        fclose(File);
    }
    return(Result);
}

static int
readMessages(can_message* message_root, char* FileName)
{
	int Result = 0;
	FILE *File = fopen(FileName, "rb");
    if(File)
    {
        size_t bytesRead = 0;
        can_message *message = (can_message*)malloc(sizeof(can_message));
        while (bytesRead = fread(message, sizeof(can_message), 1, File))
        {
 			//printf("read %zd bytes\n", bytesRead);
        	//assume wer're good.... you know what happens when you assume.
        	Assert(message->datalength);
        	message->data = (u8*)malloc(message->datalength);
        	//printf("message datalength is: %d\n", message->datalength);
        	if (message->data)
        	{
        		bytesRead = fread(message->data, sizeof(u8), message->datalength, File);
        		//printf("Read %zd bytes from the stream.\n", bytesRead);
        	}
        	else
        	{
        		printf("ERROR:didn't allocate the data pointer\n");
        	}
        	message->next = message_root->next;
        	message_root->next = message;
        	message = (can_message*)malloc(sizeof(can_message));
        	Result++;
        }
        fclose(File);
    }
    return(Result);	
}
*/



#include "config.h"
#include <inttypes.h>
#include <math.h>
#include <limits.h>
#include "libavutil/avstring.h"
#include "libavutil/mathematics.h"
#include "libavutil/imgutils.h"
#include "libavutil/dict.h"
#include "libavutil/avassert.h"
#include "libavformat/avformat.h"
#include "libavdevice/avdevice.h"
#include "libswresample/swresample.h"
#include "libswscale/swscale.h"

#include <unistd.h>
#include <assert.h>


#define NUMBER_BITS_PER_LEVEL 2
#define LEVEL_TRESHOLD 5
#define LOG_ENABLE 1
#define STOP_ON_FINISH 1


const char program_name[] = "stripped ffplay";
const int program_birth_year = 1000;
	
typedef struct VideoState {
    AVFormatContext *ic;

    int video_stream;
    AVStream *video_st;

    char filename[1024];

    struct SwsContext *img_convert_ctx;
} VideoState;


/* open a given stream. Return 0 if OK */
static int stream_component_open(VideoState *is, int stream_index)
{
    AVFormatContext *ic = is->ic;
    AVCodecContext *avctx;
    AVCodec *codec;
    AVDictionaryEntry *t = NULL;
    int64_t wanted_channel_layout = 0;

    if (stream_index < 0 || stream_index >= ic->nb_streams)
        return -1;
    avctx = ic->streams[stream_index]->codec;


    codec = avcodec_find_decoder(avctx->codec_id);
	assert (avctx->codec_type == AVMEDIA_TYPE_VIDEO);
    if (!codec)
        return -1;

    avctx->lowres = 0;
    avctx->idct_algo= FF_IDCT_AUTO;
    avctx->skip_frame= AVDISCARD_DEFAULT;
    avctx->skip_idct= AVDISCARD_DEFAULT;
    avctx->skip_loop_filter= AVDISCARD_DEFAULT;

    if(codec->capabilities & CODEC_CAP_DR1)
        avctx->flags |= CODEC_FLAG_EMU_EDGE;


    if (!codec ||
        avcodec_open2(avctx, codec, 0) < 0)
        return -1;
    if ((t = av_dict_get(0, "", NULL, AV_DICT_IGNORE_SUFFIX))) {
        av_log(NULL, AV_LOG_ERROR, "Option %s not found.\n", t->key);
        return AVERROR_OPTION_NOT_FOUND;
    }

    /* prepare audio output */

    ic->streams[stream_index]->discard = AVDISCARD_DEFAULT;
    switch(avctx->codec_type) {
    case AVMEDIA_TYPE_VIDEO:
        is->video_stream = stream_index;
        is->video_st = ic->streams[stream_index];

//        packet_queue_init(&is->videoq);
        break;
    default: assert(0);
        break;
    }
    return 0;
}

static void stream_component_close(VideoState *is, int stream_index)
{
    AVFormatContext *ic = is->ic;
    AVCodecContext *avctx;

    if (stream_index < 0 || stream_index >= ic->nb_streams)
        return;
    avctx = ic->streams[stream_index]->codec;

	assert(avctx->codec_type == AVMEDIA_TYPE_VIDEO);

    ic->streams[stream_index]->discard = AVDISCARD_ALL;
    avcodec_close(avctx);
    switch(avctx->codec_type) {
    case AVMEDIA_TYPE_VIDEO:
        is->video_st = NULL;
        is->video_stream = -1;
        break;
    default: assert(0);
        break;
    }
}

static int get_video_frame(VideoState *is, AVFrame *frame, AVPacket *pkt)
{
    int got_picture;
    avcodec_decode_video2 (is->video_st->codec, frame, &got_picture, pkt);
    return 0;
}

int my_img_convert(AVPicture* dst, enum PixelFormat dst_pix_fmt, AVPicture* src, enum PixelFormat pix_fmt, int width, int height)
{
   int av_log = av_log_get_level();
   av_log_set_level(AV_LOG_QUIET);
   struct SwsContext *img_convert_ctx = sws_getContext(width, height, pix_fmt, width, height, dst_pix_fmt, SWS_BICUBIC, NULL, NULL, NULL);
   int result = sws_scale(img_convert_ctx, src->data, src->linesize, 0, height, dst->data, dst->linesize);
   sws_freeContext(img_convert_ctx);
   av_log_set_level(av_log);
   return result;
}

void SaveFrame(AVPicture *pFrame, int width, int height, int iFrame) {
  FILE *pFile;
  char szFilename[32];
  int  y;
  
  // Open file
  sprintf(szFilename, "frame%d.ppm", iFrame);
  pFile=fopen(szFilename, "wb");
  if(pFile==NULL)
    return;
  
  // Write header
  fprintf(pFile, "P6\n%d %d\n255\n", width, height);
  
  // Write pixel data
  for(y=0; y<height; y++)
    fwrite(pFrame->data[0]+y*pFrame->linesize[0], 1, width*3, pFile);
  
  // Close file
  fclose(pFile);
}

static int queue_picture(VideoState *is, AVFrame *src_frame)
{
	AVPicture bmpp;
	AVPicture src;
	int width = is->video_st->codec->width;
	int height = is->video_st->codec->height;
	enum PixelFormat pix_fmt = is->video_st->codec->pix_fmt;
	
	src.data[0] = src_frame->data[0];
	src.data[1] = src_frame->data[1];
	src.data[2] = src_frame->data[2];
	src.linesize[0] = src_frame->linesize[0];
	src.linesize[1] = src_frame->linesize[1];
	src.linesize[2] = src_frame->linesize[2];

	avpicture_alloc (&bmpp, PIX_FMT_RGB24, width, height);
	my_img_convert (&bmpp, PIX_FMT_RGB24, &src, pix_fmt, width, height);
	SaveFrame (&bmpp, width, height, 3);
	avpicture_free (&bmpp);
    return 0;
}
short coding_started = 0;
int byte_number = 0;
int bit_number = 0;
unsigned char byte__ = 0;
unsigned int message_length = 0;

void na4uHai_Ywe_koduTj(void);
void na4uHai_Ywe_koduTj()
{
	coding_started = 1;
	byte_number = 0;
	bit_number = 0;
	byte__ = 0;
	message_length = 0;
						fclose(fopen ("buga.t", "w"));
}

static void Log (const char * format, ...)
{
    va_list ap;
    va_start(ap, format);
	char message[2048];

	if (coding_started && LOG_ENABLE)
	{
		vsnprintf (message, 2048, format, ap);
		av_log (NULL, AV_LOG_INFO, message);
	}
    va_end(ap);

}

static void finish_decode()
{
	//fclose(fi);
	
	if (!STOP_ON_FINISH)
		av_log (NULL, AV_LOG_INFO, "capable size of information: %d\n", byte_number);
}

static void draw_byte()
{
	++byte_number;
	if (byte_number < 5)
	{
		message_length |= ((unsigned int)byte__<<(8*(4 - byte_number)));
		Log ("[%u:", message_length);
		Log ("%u]\n", byte__);
	}
	if (byte_number > 4 && byte_number <= (message_length + 4))
	{
		FILE* fi = fopen ("buga.t", "a");
		fwrite(&byte__, 1, 1, fi);
		Log ("[byte[%d]: %c]\n", byte_number, byte__);
		fclose(fi);
	}
}

void push_bit (int bit)
{
	if (coding_started)
	{
		byte__ <<= 1;
		byte__ += bit;
		++bit_number;
		if (bit_number == 8)
		{
			draw_byte();
			bit_number = 0;
			byte__ = 0;
		}
	}
}

void pack_byte (int level)
{
	int mask = 1<<(NUMBER_BITS_PER_LEVEL - 1);
	Log ("%d->", level);
	while (mask > 0)
	{
		push_bit ((level&mask) != 0);
		Log ("%d", (level&mask)!=0);
		mask >>= 1;
	}
	Log ("\n");
}

void steganography_process_level (int level)
{
	if (level > LEVEL_TRESHOLD && level != 127)
	{
		pack_byte (level);
	}
}

static void work_with_video_pkt (VideoState* is, AVPacket* pkt)
{
	AVFrame *frame= avcodec_alloc_frame();
	get_video_frame (is, frame, pkt);
	queue_picture(is, frame);

	av_free(frame);
}

/* this thread gets the stream from the disk or the network */
static int read_thread (VideoState *is)
{
    AVFormatContext *ic = NULL;
    int err, i, ret;
	int st_index = 0;
    AVPacket pkt1, *pkt = &pkt1;
    int eof=0;
    AVDictionaryEntry *t;
    int orig_nb_streams;

    is->video_stream = -1;

    err = avformat_open_input(&ic, is->filename, 0, 0);
    if (err < 0) {
		fprintf (stderr, "err with file: %s, code = %i", is->filename, err);
        ret = -1;
        goto fail;
    }
    is->ic = ic;

    orig_nb_streams = ic->nb_streams;

    err = avformat_find_stream_info(ic, 0);
    if (err < 0) {
        fprintf(stderr, "%s: could not find codec parameters\n", is->filename);
        ret = -1;
        goto fail;
    }

    if(ic->pb)
        ic->pb->eof_reached= 0; //FIXME hack, ffplay maybe should not use url_feof() to test for the end

    for (i = 0; i < ic->nb_streams; i++)
        ic->streams[i]->discard = AVDISCARD_ALL;
    st_index = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    av_dump_format(ic, 0, is->filename, 0);

	na4uHai_Ywe_koduTj();

    ret=-1;
    if (st_index >= 0) {
        ret= stream_component_open(is, st_index);
    }

    if (is->video_stream < 0) {
        fprintf(stderr, "%s: could not open codecs\n", is->filename);
        ret = -1;
        goto fail;
    }

    for(;!eof;)
   	{
        ret = av_read_frame(ic, pkt);
        if (ret < 0) {
            if (ret == AVERROR_EOF || url_feof(ic->pb))
                eof=1;
            if (ic->pb && ic->pb->error)
                break;
            continue;
        }
        if (pkt->stream_index == is->video_stream)
			work_with_video_pkt(is, pkt);
       	av_free_packet(pkt);

		if ((byte_number + 4) >= message_length && STOP_ON_FINISH)
			break;
	}
	finish_decode();

    ret = 0;
 fail:

    /* close each stream */
    if (is->video_stream >= 0)
        stream_component_close(is, is->video_stream);
    if (is->ic) {
        av_close_input_file(is->ic);
        is->ic = NULL; /* safety */
    }

    return 0;
}

static VideoState *stream_open(const char *filename)
{
    VideoState *is;

    is = av_mallocz(sizeof(VideoState));
    if (!is) return;
    av_strlcpy(is->filename, filename, sizeof(is->filename));

	read_thread (is);
}
static void do_exit(VideoState *is)
{
    if (is) {
		free(is);
    }
    av_lockmgr_register(NULL);
    printf("\n");
    av_log(NULL, AV_LOG_QUIET, "%s", "");
    exit(0);
}
int main(int argc, char **argv)
{
    av_log_set_flags(AV_LOG_SKIP_REPEATED);

    /* register all codecs, demux and protocols */
    avcodec_register_all();
    av_register_all();

	if (argc < 2)
	{
		fprintf (stderr, "must provide file name");
		do_exit(0);
	}

    stream_open(argv[1]);
    /* never returns */

    return 0;
}

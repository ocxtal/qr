
/**
 * @file qr.c
 */
#include <ctype.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <qrencode.h>
#include "kvec.h"

#define BK_WH 		"\x1b[40m\x1b[37m"
#define DF_DF		"\x1b[49m\x1b[39m"
#define RV(x)		( "\x1b[7m" x "\x1b[0m" )
#define BLK 		"  "
#define LF 			"\n"

/**
 * @fn qr2ascii
 *
 * @brief convert QRcode to CLI-printable string.
 *
 * @param[in] qrcode : input QRcode
 * @param[in] margin_width : margin width
 * @return a pointer to ascii string
 */
char *qr2ascii(QRcode *qrcode, int margin_width)
{
	int i, j;
	QRcode const *qr = qrcode;
	int const m = margin_width;

	if(qr == NULL || m < 4) { return NULL; }
	int const w = qr->width;
	int const l = 2 * m + qr->width;

	char *ascii_arr = malloc(
		  strlen(BK_WH)							/* header */
		+ (strlen(RV(BLK)) * l + strlen(LF))	/* width */
		  * l									/* height */
		+ strlen("")							/* terminator */
		+ strlen(DF_DF));						/* trailer */
	char *p = ascii_arr;
	char const *b[2] = {RV(BLK), BLK};

	#define _for(cnt, len)		for(cnt = 0; cnt < len; cnt++)
	#define append(ptr, str)	{ strcpy(ptr, str); ptr += strlen(str); }
	#define color(x)			( (x) & 0x01 )

	/* header */
	append(p, BK_WH);
	/* margin at the head */
	_for(i, m) {
		_for(j, l) { append(p, b[0]); }
		append(p, LF);
	}
	/* code */
	_for(i, w) {
		_for(j, m) { append(p, b[0]); }
		_for(j, w) { append(p, b[color(qr->data[i*w+j])]); }
		_for(j, m) { append(p, b[0]); }
		append(p, LF);
	}
	/* margin at the tail */
	_for(i, m) {
		_for(j, l) { append(p, b[0]); }
		append(p, LF);
	}
	/* trailer */
	append(p, DF_DF);

	#undef _for
	#undef append
	#undef color

	return(ascii_arr);
}

/**! help message */
static
char const *help =
	LF
	"  qr -- print QR code on terminal" LF
	LF
	"  usage: $ qr [options] \"string to encode\"" LF
	"     or  $ <some program> | qr [options]" LF
	LF
	"  options:" LF
	"    -v    version   [1-40]" LF
	"    -e    EC level  [lmqh][1-4]" LF
	"    -m    mode      [na8k] (number / alphabet / 8bit / kanji)" LF
	"    -s    case sensitive mode" LF
	"    -b    margin width" LF
	"    -h    print help (this message)" LF;

/**
 * @fn main
 */
int main(int argc, char *argv[])
{
	int c;
	kvec_t(char) arr = { 0 };

	/* conversion tables */
	#define enc(x)	( (x) + 1 )	/* offset to distingish valid code with 0 */
	#define dec(x)	( (x) - 1 )
	int const et[256] = {
		['1'] = enc(QR_ECLEVEL_L), ['l'] = enc(QR_ECLEVEL_L),
		['2'] = enc(QR_ECLEVEL_M), ['m'] = enc(QR_ECLEVEL_M),
		['3'] = enc(QR_ECLEVEL_Q), ['q'] = enc(QR_ECLEVEL_Q),
		['4'] = enc(QR_ECLEVEL_H), ['h'] = enc(QR_ECLEVEL_H)
	};
	int const mt[256] = {
		['n'] = enc(QR_MODE_NUM),
		['a'] = enc(QR_MODE_AN),
		['8'] = enc(QR_MODE_8),
		['k'] = enc(QR_MODE_KANJI)
	};

	/* options and arguments */
	int version = 0;
	int ec_level = 'l';		/* QR_ECLEVEL_L */
	int mode = '8';			/* QR_MODE_8 */
	int sensitive = 0;
	int margin = 4;
	char *str = NULL;

	/* parse options */
	while(optind < argc) {
		if((c = getopt(argc, argv, "v:e:m:sb:h")) == -1) {
			str = argv[optind++]; continue;
		}
		switch(c) {
			case 'v': version = atoi(optarg); break;
			case 'e': ec_level = tolower(optarg[0]); break;
			case 'm': mode = tolower(optarg[0]); break;
			case 's': sensitive = 1; break;
			case 'b': margin = atoi(optarg); break;
			case 'h': fprintf(stderr, "%s\n", help); exit(1);
		}
	}
	if(version > 40 || et[ec_level] == 0 || mt[mode] == 0) {
		fprintf(stderr, "[ERROR] Invalid options.\n");
		exit(1);
	}

	if(optind != argc) {
		if(str != NULL) {
			fprintf(stderr, "[ERROR] Too many arguments.\n");
			exit(1);
		}
		str = argv[optind];
	}
	if(str == NULL) {
		/* pipe mode */
		kv_init(arr);
		while((c = getchar()) != EOF) {
			kv_push(arr, ((char)c));
		}
		kv_push(arr, '\0');
		str = kv_ptr(arr);
	}

	/* encode */
	QRcode *qr = QRcode_encodeString(
		str,
		version,
		dec(et[ec_level]),
		dec(mt[mode]),
		sensitive);
	if(qr == NULL) {
		printf("[ERROR] Failed to encode.\n");
		exit(1);
	}

	/* convert to ascii */
	char *ascii = qr2ascii(qr, margin);


	/* dump */
	printf("%s", ascii);
	
	/* cleanup */
	free(ascii);
	QRcode_free(qr);
	if(kv_ptr(arr) != NULL) { kv_destroy(arr); }

	return(0);
}

/**
 * end of qr.c
 */

# QR

Print QR code on the terminal.

![screenshot](https://github.com/ocxtal/qr/blob/master/screenshot.png)

## Dependency

### [libqrencode](https://github.com/fukuchi/libqrencode)

with Homebrew
	
	$ brew install qrencode

from source

	$ git clone https://github.com/fukuchi/libqrencode.git && cd libqrencode
	$ ./configure && make
	# make install

## Build

	$ gcc -std=c99 -O3 -o qr qr.c -lqrencode

## Usage

```
$ qr -h
  qr -- print QR code on terminal

  usage: $ qr [options] "string to encode"
     or  $ <some program> | qr [options]

  options:
    -v    version   [1-40]
    -e    EC level  [lmqh][1-4]
    -m    mode      [na8k] (number / alphabet / 8bit / kanji)
    -s    case sensitive mode
    -b    margin width
    -h    print help (this message)
$ qr "Hello"
or
$ echo "Hello" | qr
```

## License

MIT

Copyright (c) 2015-2016 Hajime Suzuki
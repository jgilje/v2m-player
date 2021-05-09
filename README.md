![GitHub release (latest by date)](https://img.shields.io/github/v/release/Sound-Linux-More/v2mplayer)
![GitHub Release Date](https://img.shields.io/github/release-date/Sound-Linux-More/v2mplayer)
![GitHub repo size](https://img.shields.io/github/repo-size/jgilje/v2m-player)
![GitHub all releases](https://img.shields.io/github/downloads/Sound-Linux-More/v2mplayer/total)
![GitHub](https://img.shields.io/github/license/jgilje/v2m-player)

# v2m-player
Farbrausch V2M player

![TinyPlayer](./doc/V2M-TinyPlayer.jpg)

This is a quick port of the tinyplayer at https://github.com/farbrausch/fr_public/tree/master/v2 to SDL.

## Build

```
ccmake .
cmake .
make
sudo make install
```

## Usage

```
./v2mplayer v2m/0test.v2m
zcat v2m/Dafunk--breeze.v2mz | ./v2mplayer
gzip -cdf v2m/Dafunk--breeze.v2mz | ./v2mplayer -o Dafunk--breeze.newest.v2m
```

## See also

* [FTP modland.com](http://ftp.modland.com/)

--- 
2021

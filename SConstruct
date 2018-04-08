# -*- mode: python; coding: utf-8 -*-
import pkgconfig

conf = pkgconfig.getConf()
pkgconfig.ensure(conf,'flac >= 1.3','libflac-dev')
env = conf.Finish()

env.Append(CCFLAGS = "-Wall -O0 -g -Wall -std=gnu99")
env.ParseConfig('pkg-config --cflags --libs flac')
env.Program('iqcompress',Glob('src/*.c'))

# -*- mode: python; coding: utf-8 -*-
import pkgconfig

conf = pkgconfig.getConf()
env = conf.Finish()

env.Append(CCFLAGS = "-Wall -O0 -g -Wall -std=gnu99")
env.Program('iqcompress',Glob('src/*.c'))

# English 🇬🇧/🇺🇲
Inspired in Matt Parker video "[Generating π from 1,000 random numbers](https://youtu.be/RZBhSi_PwHU)"

In case you want to run it you can do it in [the Repl.it post I created](https://repl.it/talk/share/Calculate-Pi-from-random-numbers/119909#main.cpp). 
You don't need to install nothing just click "Run" and watch it go.

Once is over is going to generate a file called PI.txt with 100 digits in case you want it. 

If you're going to download the code and run it in your machine you're going to need [Boost](https://www.boost.org/) and C++17. You may compile it in an older version.

This program runs in multithreading (5 threads by default) you can change the number of threads in the code and how many iterations is going to do,
so far is going to run 2<sup>128</sup> (340.282.366.920.938.463.463.374.607.431.768.211.456). 
It may take a time so to don't wait for it to finish it has a refresh factor, currently after 2<sup>16</sup> (65.536) iterations is going to refresh.

The console is going to print the next number after ~4 seconds.

## Pseudocode
> **let** numOfThreads = 5  
> **let** threadVector[]
> **let** math[] // vector to store the partial sum of each thread  
> **let** limit = 2<sup>128</sup>  
> **let** refreshRate = 2<sup>16</sup>  
> **for** i = 0 **up to** numOfThreads:  
> &nbsp;&nbsp;&nbsp;&nbsp; math[i] = (0,0) // each element contains how much pairs were coprimes and the total number of generated pairs  
> &nbsp;&nbsp;&nbsp;&nbsp; threadVector[i].start(i, limit/numOfThreads, refreshRate) // each thread can now start to generate numbers

Once every thread is started the main thread waits 2 seconds and iterates the **math** vector and prints the partial result according to the math showed in the video.



# Castellano 🇪🇸/🇲🇽
TODO traducirlo :v

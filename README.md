# PiFromCoprimes [![CI/CD](https://github.com/kikawet/piFromCoprimes/actions/workflows/cd-workflow.yml/badge.svg?branch=master)](https://github.com/kikawet/piFromCoprimes/actions/workflows/cd-workflow.yml)

## English 🇬🇧/🇺🇲
Inspired in Matt Parker video "[Generating π from 1,000 random numbers](https://youtu.be/RZBhSi_PwHU)"

In case you want to run it you can do it in [the Repl.it post I created](https://repl.it/talk/share/Calculate-Pi-from-random-numbers/119909#main.cpp). 
You don't need to install nothing just click "Run" and watch it go.

Once is over is going to generate a file called _PI.txt_ with 100 digits in case you want it. 

If you're going to download the code and run it in your machine you're going to need [Boost](https://www.boost.org/) and C++17.

This program runs in multithreading (5 threads by default) you can change the number of threads in the code and how many iterations is going to do,
so far is going to run 2<sup>128</sup> (340.282.366.920.938.463.463.374.607.431.768.211.456). 
It may take a time so to don't wait for it to finish it has a refresh factor, currently after 2<sup>16</sup> (65.536) iterations is going to refresh.

The console is going to print the next number after ~4 seconds.

### Pseudocode
> **let** numOfThreads = 5  
> **let** threadVector[]
> **let** math[] // vector to store the partial sum of each thread  
> **let** limit = 2<sup>128</sup>  
> **let** refreshRate = 2<sup>16</sup>  
> **for** i = 0 **up to** numOfThreads:  
> &nbsp;&nbsp;&nbsp;&nbsp; math[i] = (0,0) // each element contains how much pairs were coprimes and the total number of generated pairs  
> &nbsp;&nbsp;&nbsp;&nbsp; threadVector[i].start(i, limit/numOfThreads, refreshRate) // each thread can now start to generate numbers

Once every thread is started the main thread waits 2 seconds and iterates the **math** vector and prints the partial result according to the math showed in the video.



## Castellano 🇪🇸/🇲🇽
Inspirado por el video de Matt Parker "[Generating π from 1,000 random numbers](https://youtu.be/RZBhSi_PwHU)"

Si quieres ejecutar el proyecto he creado un [post en Repl.it](https://repl.it/talk/share/Calculate-Pi-from-random-numbers/119909#main.cpp).
No hace falta instalarse nada, para ejecutarlo solo darle a ejecutar en la página y disfrutar.

Cuando termine se generará un fichero llamado _PI.txt_ con las 100 primeros digitos calculados.

En caso de quere descargar el proyecto para ejecutarlo en local es necesario tener instalado [Boost](https://www.boost.org/) y C++17.

La aplicación se ejecuta en paralelo (5 hilos por defecto) se puede cambiar el número de hilos dentro del código así como el número total de iteracionesa ejecutar, 
por defecto se calculan 2<sup>128</sup> (340.282.366.920.938.463.463.374.607.431.768.211.456) iteraciones.
Como puede tardar en calcular todas esas iteraciones hay un factor interno para calcular las variables, inicialmente vale 2<sup>16</sup> (65.536).

Cada ~4 segundos se muestra la siguiente estimación con los números generados hasta el momento.
### Pseudocódigo
> **let** numDeHilos = 5  
> **let** vectorHilos[]
> **let** mates[] // vector para almacenar las sumas parciales de cada hilo 
> **let** limite = 2<sup>128</sup>  
> **let** tasaActualiza = 2<sup>16</sup>  
> **for** i = 0 **hasta** numDeHilos:  
> &nbsp;&nbsp;&nbsp;&nbsp; mates[i] = (0,0) // cada elemento almacena cuantos pares de coprimos y el total de pares generados 
> &nbsp;&nbsp;&nbsp;&nbsp; threadVector[i].start(i, limite/numDeHilos, tasaActualiza) // cada hilo puede empezar a generar números

Cuando todos los hilos se han empezado the hilo principal espera 2 segundos e itera el vector **mates** y muestra por pantalla el resultado parcial basandose en las equaciones que se muestran en el video.

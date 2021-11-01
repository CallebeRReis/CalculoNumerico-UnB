#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdio.h>
#include <iomanip>

#pragma once

// Realiza o calculo direto de uma regra do trapezio, não subdividindo em intervalos.
// função necessita de dois parâmetros => x,y, sendo que y=f(x).
// A fórmula geral do método é: integral(f(x))[a,b] = (1/2*f(a)+1/2*f(b))*h
// h refere-se a uma variação desejada.
// Caso divida o intervalo em n, sua equação será: integral(f(x))[a,b] = (1/(2*n)*f(a)+1/(2*n)*f(b))*h
double regraTrapezio(void(*funcao)(double&, double&), double inicioInter, double fimInter, double numeroSubintervalos=1,double h=1);
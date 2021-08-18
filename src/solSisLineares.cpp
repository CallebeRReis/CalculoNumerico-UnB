#include"solSisLineares.hpp"

void quadrado(double& x, double& resultado)
{
    resultado = x*x;
}

// Para derivadas de R^n -> R
// Fórmula da derivada (f(x+h)-f(x-h))/(2*h) (https://en.wikipedia.org/wiki/Numerical_differentiation)
// Erro cai com h^2 (é proporcional, mas não é igual) 
void derivada(void(*funcao)(double&, double&), double& ponto, double step, double& derivadaNoPonto)
{  
    double pontoMais  = ponto + step;
    double pontoMenos = ponto - step;
    double fMais,fMenos;

    funcao(pontoMais ,fMais);
    funcao(pontoMenos,fMenos);

    derivadaNoPonto = (fMais -fMenos)/(2.0*step);
}

// Metodo de Newton Raphson para função bem comportada de R -> R 
// Fórmula para o método x_i+1 = x_i + f(x_i)/f'(x_i) (https://en.wikipedia.org/wiki/Newton%27s_method)
// Erro caí com h^2  
void newtonRaphson(void(*funcao)(double&, double&), double condicaoInicial,
                   long long unsigned int maxIteracoes, double precisaoDaRaiz, double& resultado)
{
    double auxCondInicial = condicaoInicial;
    double resultadoDerivada;
    double precisaoDerivada = 1e-10;

    // Chute inicial
    double auxValorFuncao;
    funcao(auxCondInicial, auxValorFuncao);

    long long unsigned int i = 0;
    while(i < maxIteracoes && fabs(auxValorFuncao) >= precisaoDaRaiz)
    {
        derivada(funcao, condicaoInicial, precisaoDerivada, resultadoDerivada);

        if(resultadoDerivada==0)
        {
            condicaoInicial = INFINITY;
            std::cout << "[ERRO] Derivada igual a zero.\n";
            break;
        }
        else if(std::isnan(resultadoDerivada))
        {
            condicaoInicial = INFINITY;
            std::cout << "[ERRO] Nao e um numero\n";
            break;
        }

        funcao(condicaoInicial,auxValorFuncao);
        condicaoInicial = condicaoInicial - auxValorFuncao/resultadoDerivada;

        funcao(condicaoInicial, auxValorFuncao);
        i++;
    }

    // Resetar a precisão
    std::cout.precision(precisao_padrao);

    std::cout<<"partindo do ponto inicial x = "<<auxCondInicial<< "\n";
    std::cout<<"foram feitas " <<i<<" iteracoes"<<"\n";
    std::cout<<"com uma precisao de: "<<precisaoDaRaiz<<"\n";

    resultado = condicaoInicial;
    std::cout.precision(-log10(precisaoDaRaiz));
    std::cout<<"a raiz encontrada foi: "<<resultado<<std::endl;

    // Alternativamente:
    // printf("%.20lf\n", resultado);
}

// Metódo do ponto fixo para função de R -> R
// fórmula para o metódo x_i+1 = f(x_i), 
// para a convergência do metódo é necessário que |f'(x)| < 1 
// (Por isso, a forma como se escreve f(x) vai ter um papel muito importante na utilização do metódo)
// fórmula para o erro carece de fontes, mas experimentalmente, parece concordar bem

void pontoFixo(void (*funcao)(double&, double&), double condicaoInicial,
                   uint maxIteracoes, double raizDaPrecisao, double& resultado)
{
    double auxValorFuncao, auxPrecisao, auxCondInicial = condicaoInicial;
    uint i = 0;
    while( (i < maxIteracoes)  & !(std::abs(auxPrecisao - condicaoInicial) < raizDaPrecisao*raizDaPrecisao) )
    {
        auxPrecisao = condicaoInicial;
        funcao(condicaoInicial, auxValorFuncao);
        condicaoInicial = auxValorFuncao ;
        i++;
    }

    // Resetar a precisão
    std::cout.precision(precisao_padrao);

    resultado = condicaoInicial;
    std::cout<<"partindo do ponto inicial x = "<<auxCondInicial<< "\n";
    std::cout<<"foram feitas " <<i<<" iteracoes"<<"\n";
   
    std::cout<<"com uma precisao de: "<<pow(std::abs(auxPrecisao- condicaoInicial),2)<<"\n";
    std::cout.precision(std::abs(2.0 * std::log(std::abs(auxPrecisao - condicaoInicial))));

    resultado = condicaoInicial;
    std::cout<<"a raiz encontrada foi: "<<std::fixed<<resultado<<std::endl;
}

// Método da Posição Falsa [ Definido para  R -> R ]
// Para convergência é necessário que em um intervalo [x0, x1], é necessário que f(x0).f(x1) < 0 (Teorema de Bolzano).
// Fórmula do Método: xn = x1 - f(x1)(x1 - x0)/(f(x1) - f(x0)) => (A diferença para o método da secante é o fato deste utilizar dois pontos).
// Fontes: https://en.wikipedia.org/wiki/Regula_falsi
//         https://pt.wikipedia.org/wiki/M%C3%A9todo_da_posi%C3%A7%C3%A3o_falsa

void posicaoFalsa(void(*funcao)(double&, double&), double condicaoInicial_0, double condicaoInicial_1, 
    long long unsigned int maxIteracoes, double precisao, double& resultado)
{
    double condicao_0 = condicaoInicial_0, condicao_1 = condicaoInicial_1;
    double resultado_0, resultado_1;

    funcao(condicao_0, resultado_0);
    funcao(condicao_1, resultado_1);

    if(resultado_0 * resultado_1 >= 0)
    {
        std::cout<<"[ERRO] Condicoes iniciais erroneas.\n";
        resultado =  INFINITY;
        return;
    }

    // Garantir que x0 < x1, no intervalor [x0, x1]
    if(resultado_0 > resultado_1)
    {
        double aux = condicaoInicial_0;
        condicaoInicial_0 = condicaoInicial_1;
        condicaoInicial_1 = aux;
    }

    double condicaoAtual=condicaoInicial_1;
    
    double valorFuncao;
    funcao(condicaoAtual, valorFuncao);

    long long unsigned int i=0;
    while(i < maxIteracoes && fabs(valorFuncao)>precisao)
    {
        if(condicao_1 - condicao_0 == 0)
        {
            std::cout << "[ERRO] No intervalo [a,b], a = b.\n";
            condicaoAtual = INFINITY;
            break;
        }

        funcao(condicao_0, resultado_0);
        funcao(condicao_1, resultado_1);

        condicaoAtual = condicao_1-resultado_1*(condicao_1-condicao_0)/(resultado_1-resultado_0);
        funcao(condicaoAtual, valorFuncao);

        if(valorFuncao*resultado_0<0)
            condicao_1 = condicaoAtual;
        else
            condicao_0 = condicaoAtual;

        i++;
    }

    // Resetar a precisão
    std::cout.precision(precisao_padrao);

    std::cout<<"\n| METODO DA POSICAO FALSA |\n";
    std::cout<<"Equacao: xn = x1 - f(x1)(x1 - x0)/(f(x1) - f(x0))\n";
    std::cout<<"Condicoes Iniciais: x0 = "<<condicaoInicial_0<<", x1 = "<<condicaoInicial_1<<"\n";
    std::cout<<"Iteracoes Feitas: "<<i<<"\n";
    std::cout<<"Precisao: "<<precisao<<"\n";

    std::cout.precision(-log(precisao));
    std::cout<<"Raiz encontrada: "<<condicaoAtual<<"\n\n";

    resultado = condicaoAtual;
}

void bissecao(void(*funcao)(double&, double&), double inicioInter, double fimInter, long long unsigned int maxIteracoes,
double precisao, double& resultado)
{
    double xMin=inicioInter;
    double xMax=fimInter;
    
    double yMin, yMax;
    funcao(xMin, yMin);
    funcao(xMax, yMax);

    if(yMin*yMax>0)
    {
        std::cout<<"[ERRO] Intervalo Invalido.\n";
        resultado =  INFINITY;
        return;
    }

    if(yMin>0)
    {
        double aux=xMin;
        xMin = xMax;
        xMax = aux;
    }

    double xMedio = xMax;
    double yMedio;
    funcao(xMedio, yMedio);

    long long unsigned int i=0;
    while(fabs(yMedio)>precisao && i < maxIteracoes)
    {
        if(yMedio*yMin<0)
            xMax = xMedio;
        else
            xMin = xMedio;
        xMedio = (xMin + xMax)/2.0;
        funcao(xMedio,yMedio);
        i++;
    }

    // Resetar a precisão
    std::cout.precision(precisao_padrao);

    std::cout<<"| METODO DA BISSECAO |\n";
    std::cout<<"Equacao: xn = (x1+x2)/2\n";
    std::cout<<"Iteracoes Feitas: "<<i<<"\n";
    std::cout<<"Precisao: "<<precisao<<"\n";

    std::cout.precision(-log(precisao));
    std::cout<<"Raiz encontrada: "<<xMedio<<"\n\n";

    resultado = xMedio;
}
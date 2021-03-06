#include"solSisLineares.hpp"

//==========================================================================
//|                   S O L U Ç Õ E S - A L G É B R I C A S                |
//==========================================================================

void quadrado(double& x, double& resultado)
{
    resultado = x*x;
}

void bolzano(void(*funcao)(double&,double&), double& minIntervalo, double& maxIntervalo,
long long unsigned int maxIteracoes, double passo)
{
    // !!! Possui instabilidades => Deverá ser feita uma analise mais aprofundada.
    // Aplica Teorema de bolzano para determinar um intervalo onde pode existir raiz.
    // minIntervalo e maxIntervalo armazenam os valores mínimo e máximo do intervalo.

    // Parte da busca irá ocorrer para x positivo e outra negativo, alternando entre os dois.

    bool encontrouPositivo = false, encontrouNegativo = false; // Sinais Encontrados
    long long unsigned int iteracoesRestantes = maxIteracoes;
    long long unsigned int subIteracoes = maxIteracoes/10;
    long long unsigned int i=0;

    double xNeg=-passo,xPos=passo;
    double yResultante;

    while(iteracoesRestantes>0)
    {
        // Busca na parte dos positivos
        i = 0;
        while(i<subIteracoes && !(encontrouPositivo && encontrouNegativo))
        {
            funcao(xPos, yResultante);
            if(yResultante>0 && !encontrouPositivo)
            {
                encontrouPositivo = true;
                maxIntervalo = xPos;
            }
            else if(yResultante < 0 && !encontrouNegativo)
            {
                encontrouNegativo = true;
                minIntervalo = xPos;
            }

            xPos+= passo;
            i++;
        }

        if(encontrouPositivo && encontrouNegativo) break;

        // Busca na parte dos Negativos
        i = 0;
        while(i<subIteracoes && !(encontrouPositivo && encontrouNegativo))
        {
            funcao(xNeg, yResultante);
            if(yResultante>0 && !encontrouPositivo)
            {
                encontrouPositivo = true;
                maxIntervalo = xNeg;
            }
            else if(yResultante < 0 && !encontrouNegativo)
            {
                encontrouNegativo = true;
                minIntervalo = xNeg;
            }

            xNeg-= passo;
            i++;
        }

        iteracoesRestantes -= 2*subIteracoes;
    }

    if(minIntervalo > maxIntervalo)
    {
        double aux = minIntervalo;
        minIntervalo = maxIntervalo;
        maxIntervalo = aux;
    }
}
void lorenz(std::vector<double>& x, std::vector<double>&fx)
{
    double sigma = 10.0;
    double rho = 8.0/3.0;
    double beta = 28.0;
    fx[0] = (sigma*(x[1]-x[0]));
    fx[1] = (x[0]*(rho - x[2]) - x[1]);
    fx[2] = (x[0]*x[1] - beta*x[2]);
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
void jacobiano(void(*funcao)(std::vector<double>&,std::vector<double>&),
                    std::vector<double>& ponto, double step,
                    uint dimImagem, Matriz& jacobianoNoPonto)
{
    std::vector<double> auxValorMais(dimImagem); 
    std::vector<double> auxValorMenos(dimImagem); 
    
    unsigned int dimDominio = ponto.size();
    std::vector<double> auxMais = ponto;
    std::vector<double> auxMenos = ponto; 
    
    // std::thread f1;
    // std::thread f2;

    for(uint j = 0; j < dimImagem; j++)
    {
        for(uint i = 0; i < dimDominio; i++)
            {
                // x_i +/- h
                auxMais[i] += step;
                auxMenos[i] -= step;
                // (f(x1,x2, ... ,x_i+h,..., xn)-f(x1,x2, ... ,xi-h,..., xn))/(2*h)

                // Essas threads fazem o programa rodar mais devagar, se souber resolver, fique a vontade

                // std::thread f1(funcao,std::ref(auxMais),std::ref(auxValorMais));
                // std::thread f2(funcao,std::ref(auxMenos),std::ref(auxValorMenos));
                // f1.join();
                // f2.join();

                funcao(auxMais,auxValorMais);
                funcao(auxMenos,auxValorMenos);
                //J(i,j) = D_xi(f_j)
              
                jacobianoNoPonto.elementos[j][i] = (auxValorMais[j] - auxValorMenos[j])/(2*step);
                
                //desfaz x_i +/- h 
                auxMais[i] -= step;
                auxMenos[i] += step;
            }        
    }    
}
void funcTeste(std::vector<double>& x,std::vector<double>& fx)
{
    fx[0] = x[0]*x[0]/3.0  + x[1]*x[1] - 1.0;
    fx[1] = x[0]*x[0]  + x[1]*x[1]/4.0 - 1.0;
}
void newtonRapshonSistemas(void(*funcao)(std::vector<double>&,std::vector<double>&),
                           std::vector<double>& chuteInicial, double step, uint maxIteracoes)
{
    Matriz J;
    criarMatriz(J,chuteInicial.size(),chuteInicial.size());
    std::vector<double> aux(chuteInicial.size());
    std::vector<double> F(chuteInicial.size());
    Matriz Jinversa; 
    std::vector<double> delta(chuteInicial.size());
    // std::vector<double> aux = chuteInicial;
    double erro = 1; 
    double det;
    uint i = 0;
    while((i < maxIteracoes)&(erro >= 1e-5))
    // for(uint i = 0; i < maxIteracoes; i++)
    {
        
        aux = chuteInicial;
        jacobiano(funcao, chuteInicial,step,chuteInicial.size(),J);
        // inversa(J,Jinversa);
        funcao(chuteInicial,F);
        // multiplicarVetorNumero(F,-1.0,F);        
        resolveSistema(J,F,delta,det);
        
        // exibirVetor(delta);
        // std::cout<<" ";

        // multiplicaVetorMatrix(Jinversa,F, aux);
        substraiVetores(chuteInicial,delta,chuteInicial);

        substraiVetores(aux,chuteInicial,aux);
        normaVetor(aux, erro);
        i++;

        std::cout<<i<<", erro:"<< erro <<"\n";

    }
}
// Metodo de Newton Raphson para função bem comportada de R -> R 
// Fórmula para o método x_i+1 = x_i + f(x_i)/f'(x_i) (https://en.wikipedia.org/wiki/Newton%27s_method)
// Erro caí com h^2  
void newtonRaphson(void(*funcao)(double&, double&), double condicaoInicial,
                   uint maxIteracoes, double raizDaPrecisao, double& resultado)
{
    double auxCondInicial = condicaoInicial;
    double resultadoDerivada;
    double auxPrecisao;

    // Chute inicial
    double auxValorFuncao;
    funcao(auxCondInicial, auxValorFuncao);

    uint i = 0;
    while( (i < maxIteracoes) & (fabs(auxValorFuncao-auxPrecisao) >= raizDaPrecisao*raizDaPrecisao))
    {
        derivada(funcao, condicaoInicial, raizDaPrecisao, resultadoDerivada);
        auxPrecisao = auxValorFuncao;
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
    std::cout<<"com uma precisao de: "<<raizDaPrecisao<<"\n";

    resultado = condicaoInicial;
    std::cout.precision(std::abs(log10(raizDaPrecisao)));
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
    double auxPrecisao;
    long long unsigned int i=0;
    while(i < maxIteracoes && fabs(valorFuncao-auxPrecisao)>precisao)
    {
        auxPrecisao = valorFuncao;
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
    std::cout<<"Precisao: "<<fabs(valorFuncao-auxPrecisao)<<"\n";

    std::cout.precision(fabs(log(fabs(valorFuncao-auxPrecisao))));
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
    std::cout<<"Precisao: "<<fabs(yMedio)<<"\n";

    std::cout.precision(-log(fabs(yMedio)));
    std::cout<<"Raiz encontrada: "<<xMedio<<"\n\n";

    resultado = xMedio;
}

//==========================================================================
//|             O P E R A Ç Õ E S - C O M - M A T R I Z E S                |
//==========================================================================

void trocaLinhas(Matriz &M, long long unsigned linha1, long long unsigned linha2)
{
    std::swap(M.elementos[linha1],M.elementos[linha2]);
}

std::vector<double> multiplicaLinhaEscalar(Matriz M, long long unsigned linha, double escalar)
{
    std::vector<double> resultante(M.colunas, 0);

    long long unsigned i=0;
    for(auto elemento:M.elementos[linha])
    {
        resultante[i]=elemento*escalar;
        i++;
    }

    return resultante;
}

void subtrairLinhas(Matriz &M, long long unsigned linhaModificada, long long unsigned linhaAuxiliar, double escalarAuxiliar)
{
    std::vector<double> novaLinha = M.elementos[linhaModificada];
    std::vector<double> novaLinhaAuxiliar = multiplicaLinhaEscalar(M,linhaAuxiliar,escalarAuxiliar);

    for(long long unsigned i=0;i<M.colunas;++i)
        novaLinha[i] = novaLinha[i]-novaLinhaAuxiliar[i];
    M.elementos[linhaModificada] = novaLinha;
}

void pivoteamentoParcial(Matriz &L, Matriz &U, Matriz& P, long long unsigned linhaPivoteada, long long unsigned colunaPivoteada)
{
    if(U.elementos[linhaPivoteada][colunaPivoteada]!=0)
        return;
    
    long long unsigned linhaAux = linhaPivoteada;
    for(long long unsigned i=linhaPivoteada+1;i<U.linhas;++i)
    {
        if(U.elementos[i][colunaPivoteada]!=0)
        {
            linhaAux = i;
            trocaLinhas(U,linhaPivoteada,i);
            trocaLinhas(P,linhaPivoteada,i);
            break;
        }
    }

    // Mudança na Matriz L
    if(linhaAux!=linhaPivoteada)
    {
        double aux;
        for(long long unsigned j=0;j<colunaPivoteada;++j)
        {
            aux = L.elementos[linhaPivoteada][j];
            L.elementos[linhaPivoteada][j] = L.elementos[linhaAux][j];
            L.elementos[linhaAux][j] = aux;
        }
    }
}

void fatoraLU(Matriz M, Matriz &L, Matriz &U, Matriz& P)
{
    U=M;
    Identidade(L, M.linhas);
    Identidade(P, M.linhas);

    for(long long unsigned i=0;i<U.linhas;++i)
    {
        pivoteamentoParcial(L,U,P,i,i);

//         std::cout<<"Matriz L:\n";
//         exibirMatriz(L);
//         std::cout<<"\n";

//         std::cout<<"Matriz: U\n";
//         exibirMatriz(U);
//         std::cout<<"\n";
// 
//         std::cout<<"Matriz: P\n";
//         exibirMatriz(P);

//         std::cout<<"==================\n";

        for(long long unsigned j=i+1;j<U.colunas;++j)
        {
            if(U.elementos[j][i]==0) continue;

            L.elementos[j][i]=U.elementos[j][i]/U.elementos[i][i];
            subtrairLinhas(U,j,i,U.elementos[j][i]/U.elementos[i][i]);
        }
    }
}
void resolveLU(Matriz& L, Matriz& U, Matriz& P, std::vector<double>& b,std::vector<double>& resultado)
{
    std::vector<double> y (L.linhas,0);
    double soma = 0;
    std::vector<double> Pb; 
    multiplicaVetorMatrix(P,b,Pb);

    for(uint j = 0; j < y.size();j++)
    {    
        soma = 0;
        for(uint i = 0; i < j; i++)
        {
            soma += L.elementos[j][i]*y[i];
        }
        y[j] = (Pb[j] - soma);
    }

    for(int i = U.linhas-1; i >=0; i--)
    {
        soma = 0;
        for(uint k = 1+i; k < U.colunas; k++)
        {
            soma += U.elementos[i][k]*resultado[k];
        }
        resultado[i] = (y[i] - soma) / U.elementos[i][i] ;
    }

}
void determinante(Matriz& A, double& det)
{
    std::vector<double> b(A.linhas,0); 

    Matriz L,U,P;
    fatoraLU(A,L,U,P);
    det = 1;
    for(uint i = 0; i < U.linhas; i ++)
    {
        det *= U.elementos[i][i];
    }
 
}
void resolveSistema(Matriz& A, std::vector<double>& ladoDireito, std::vector<double> & solucao, double& det)
{
    double deter;
    determinante(A, deter);
    if(deter==0)
    {
        std::cout<<"Sistema não tem solução!"<<"\n"; 
        det = deter;
    }
    else
    {
        Matriz L,U,P;
        fatoraLU(A,L,U,P);
        resolveLU(L,U,P, ladoDireito, solucao); 
        det = deter;
    }
}

void inversa(Matriz& A, Matriz& inversa)
{
    criarMatriz(inversa, A.linhas, A.colunas);

    std::vector<double> colunaIdentidade[A.colunas];
    std::vector<double> colunaSolucao[A.colunas];

    for(long long unsigned j=0;j<A.colunas;++j)
    {
        std::vector<double> vecAux(A.linhas, 0);
        
        colunaSolucao[j] = vecAux;

        vecAux[j] = 1;
        colunaIdentidade[j] = vecAux;
    }

    // Descobrindo colunas da matriz inversa
    double d;
    for(long long unsigned j=0;j<A.colunas;++j)
        resolveSistema(A, colunaIdentidade[j], colunaSolucao[j], d);

    // Definindo elementos da matriz inversa
    for(long long unsigned i=0;i<inversa.linhas;++i)
    {
        for(long long unsigned j=0;j<inversa.colunas;++j)
            inversa.elementos[i][j] = colunaSolucao[j][i];
    }
}

void verificarSol(Matriz& A, std::vector<double>& b, std::vector<double> solucao)
{
    std::vector<double> aux;
    multiplicaVetorMatrix(A,solucao,aux);

    std::cout<<"Comparações:\n";
    for(long long unsigned i=0;i<A.linhas;++i)
        std::cout<<"["<<i+1<<"] "<<aux[i]<<" <=> "<< b[i]<<"\n";
}
void autoValorPotencia(Matriz& A, std::vector<double>& chuteInicial,
                        uint maxIteracoes,double tol, double& erroFinal,
                        double& maiorAutoValor)
{
    double erro=1.0;
    std::vector<double>erroAux(chuteInicial.size());
    uint i = 0;
    std::vector<double> aux (chuteInicial.size());
    while((i<=maxIteracoes)&(erro>=tol))
    {
        multiplicaVetorMatrix(A,chuteInicial,aux);
        produtoInterno(chuteInicial,aux,maiorAutoValor);
        normalizarVetor(aux,aux);
        substraiVetores(chuteInicial,aux, erroAux);
        normaVetor(erroAux, erro);
        i++;
        chuteInicial = aux; 
        erroFinal = erro;
    }
}
void decompoiscaoLDU(Matriz& A, Matriz& L,Matriz& D,Matriz& U)
{
    criarMatriz(L, A.linhas, A.colunas);
    criarMatriz(D, A.linhas, A.colunas);
    criarMatriz(U, A.linhas, A.colunas);

    for(uint i = 0; i < A.colunas; i++)
    {   
  
        for(uint j = A.linhas-1; j > i; j--)
        {
            L.elementos[j][i] = A.elementos[j][i];
        }
        for(uint j = i; j < A.linhas; j--)
        {
            U.elementos[j][i] = A.elementos[j][i];
        }
        L.elementos[i][i] = 0.0;
        U.elementos[i][i] = 0.0;

        D.elementos[i][i] = A.elementos[i][i];
    }
}
void criarMatrizIteracaoGaussSidel(Matriz& A, std::vector<double>& b, 
                                   Matriz&matrizIteracao, std::vector<double>& vetorIteracao)
{
    Matriz L,D,U; 
    Matriz Aux;
    Matriz AuxMult;
    Matriz Aux2; 
    vetorIteracao.resize(b.size());

    // Achar T_g

    decompoiscaoLDU(A,L,D,U); 
    somaMatrizes(L,D, matrizIteracao);
    inversa(matrizIteracao,Aux);
    multiplicaMatrizes(Aux,U,AuxMult);
    multiplicarMatrizNumero(AuxMult,-1,Aux2);   
    matrizIteracao = Aux2;
    
    //Achar C_g
    multiplicaVetorMatrix(Aux,b,vetorIteracao);
}
void gaussSidel(Matriz& A, std::vector<double>&b, std::vector<double>& chuteInicial,
                std::vector<double>& resultado, uint maxIteracoes, double tol)
{
    resultado.resize(b.size());
    Matriz matrizIteracao;
    std::vector<double> vetorIteracao;
    criarMatrizIteracaoGaussSidel(A,b,matrizIteracao,vetorIteracao);
    double erro = 1;
    uint i = 0;
    std::vector<double> aux = chuteInicial;
    while((i<= maxIteracoes)&(erro >= tol))
    {
        aux = resultado;
        multiplicaVetorMatrix(matrizIteracao,chuteInicial,resultado);
        somaVetores(resultado,vetorIteracao, resultado);

        substraiVetores(aux,resultado,aux);
        normaVetor(aux, erro);
        // normaEuclidianaResiduo(matrizIteracao,vetorIteracao,resultado, erro);
        // std::cout<<erro<<"\n";
        chuteInicial= resultado;
        std::cout<<i<<", erro:"<< erro <<"\n";
        exibirVetor(resultado);
        std::cout<<"\n";

        i++;
    }
}
void criarMatrizIteracaoSOR(Matriz& A, std::vector<double>& b, 
                            Matriz&matrizIteracao, std::vector<double>& vetorIteracao)
{
    double omega = 0.5;
    Matriz L,D,U; 
    Matriz Aux;
    Matriz AuxMult;
    Matriz Aux2; 
    Matriz Aux3;
    vetorIteracao.resize(b.size());

    // Achar T_g

    decompoiscaoLDU(A,L,D,U); 
    multiplicarMatrizNumero(L,omega);
    somaMatrizes(L,D, matrizIteracao);
    inversa(matrizIteracao,Aux);
    multiplicarMatrizNumero(D,(1.0-omega));
    multiplicarMatrizNumero(U,(-omega));
    somaMatrizes(D,U,Aux3);
    multiplicaMatrizes(Aux,Aux3,AuxMult);
 
    matrizIteracao = AuxMult;
    
    //Achar C_g
    multiplicaVetorMatrix(Aux,b,vetorIteracao);
    multiplicarVetorNumero(vetorIteracao,omega);
}
void iteracaoSOR(Matriz& A, std::vector<double>&b, std::vector<double>& chuteInicial,
                std::vector<double>& resultado, uint maxIteracoes, double tol)
{
    resultado.resize(b.size());
    Matriz matrizIteracao;
    std::vector<double> vetorIteracao;
    criarMatrizIteracaoSOR(A,b,matrizIteracao,vetorIteracao);
    double erro = 1;
    uint i = 0;
    std::vector<double> aux = chuteInicial;
    while((i<= maxIteracoes)&(erro >= tol))
    {
        multiplicaVetorMatrix(matrizIteracao,chuteInicial,resultado);
        somaVetores(resultado,vetorIteracao, resultado);

        substraiVetores(aux,resultado,aux);
        normaVetor(aux, erro);
        // normaEuclidianaResiduo(matrizIteracao,vetorIteracao,resultado, erro);
        // std::cout<<erro<<"\n";
        chuteInicial= resultado;
        std::cout<<i<<", erro:"<< erro <<"\n";
        exibirVetor(resultado);
        std::cout<<"\n";
        i++;

    }
    std::cout<<i<<"\n";
}
void normaEuclidianaResiduo(Matriz& A, std::vector<double>& b, std::vector<double>& x, double& norma)
{
    std::vector<double> aux (x.size());
    std::vector<double> aux2 (x.size());

    multiplicaVetorMatrix(A, x,aux);
    substraiVetores(b,aux, aux2);
    normaVetor(aux2, norma);
}
void raioEspectral(Matriz& A, double& raio)
{
    std::vector<double> aux (A.colunas);
    raio = 0;
    for(uint i = 0; i < A.linhas; i++)
    {
        for(uint j = 0; j< A.colunas; j++)
        {
            aux[j] = std::fabs(A.elementos[i][j]); 
        }
        auto it = max_element(std::begin(aux), std::end(aux));
        raio += *it;
    }    
}
void extrapolacaoRichardsonDerivada(void(*funcao)(double&, double&), double& ponto,
                                    double step, double t, double& derivadaNoPonto)
{
    double derivadaAux1;
    double derivadaAux2;
    // std::thread t1(derivada,std::ref(funcao),std::ref(ponto),step/t,std::ref(derivadaAux1));
    // std::thread t2(derivada,std::ref(funcao),std::ref(ponto),step,std::ref(derivadaAux2));
    derivada(funcao,ponto,step/t,derivadaAux1);
    derivada(funcao,ponto,step,derivadaAux2);

    // t1.join();
    derivadaAux1*=pow(t,2);
    // t2.join();
    derivadaNoPonto = (derivadaAux1 - derivadaAux2)/(pow(t,2)-1);
}
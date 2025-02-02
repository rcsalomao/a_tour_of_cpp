#include <math.h>

#include <print>
#include <vector>

void print(auto&& v) { std::println("{}", v); }

namespace capitulo_1 {
constexpr double square(double x) { return x * x; }
consteval double square2(double x) { return x * x; }

void print_square(double x) {
    std::println("The square of {} is {}", x, square(x));
}

// >>> Uma função não pode ser invocada, a menos
// que tenha sido previamente declarada.

struct Elem {
    // Uma função pode também ser membro de uma classe:
    char& operator[](int index);
};
Elem* next_elem();
void exit(int);
double sqrt(double);

// E que pode ser definida fora do local de declaração da mesma (o nome da
// classe, passa a fazer parte do nome da função):
char& Elem::operator[](int index) {};

// Escopos:
struct Record {
    std::string name;  // atributo 'name' é membro de 'Record'.
};
std::vector<int> vec;  // vec é uma variável global
void fct(int arg) {    // fct possui escope global
                       // arg é um parâmetro de escopo local (função)
    std::string motto{"Who dares, wins"};  // motto é variável local
    auto p = new Record{
        "Chico Science"};  // ponteiro 'p', de escopo local, aponta para
                           // variável sem nome 'Record', criada por 'new'.
    delete p;  // Um objeto deve ser construído (inicializado) antes de ser
               // usado e será destruído ao fim do seu escopo. Para objetos de
               // escopo de 'namespace', o momento de sua destruição é o fim da
               // execução do programa. Para atributos membros, o momento de sua
               // destruição é determinado pelo momento de destruição do objeto
               // que o guarda. Um Objeto de classe T criado por 'new',
               // sobreviverá até ser destruído por 'delete'.
}

double sum(const std::vector<double>& v) {
    double res = 0;
    for (auto& i : v) {
        res += i;
    }
    return res;
}

void main() {
    std::println("{}", "Hello world!");
    print_square(1.234);
    // double s2 = sqrt(2);  // Não vai dar erro de compilação, mas como sqrt
    // não foi definida, irá dar erro de execução. double s3 = sqrt("three"); //
    // Erro de compilação. Função recebe argumento do tipo double, e não do tipo
    // literal.

    // Operadores:
    int x = 3;
    int y = 4;
    // operadores aritméticos:
    print(x + y);
    print(+x);
    print(x - y);
    print(-x);
    print(x * y);
    print(x / y);
    print(x % y);
    // operadores de comparação:
    print(x != y);
    print(x == y);
    print(x < y);
    print(x > y);
    print(x <= y);
    print(x >= y);
    // operadores lógicos:
    print(x & y);   // bitwise and
    print(x | y);   // bitwise or
    print(x ^ y);   // bitwise exclusive or
    print(~x);      // bitwise complement
    print(x && y);  // logical and
    print(x || y);  // logical or
    print(!x);      // logical not
                    // operadores bitwise retornam valor com o mesmo tipo do
                    // operando cuja operação foi realizada.
                    //
    // conversões de tipo, em operações aritméticas, são realizadas
    // de forma que possam ser livremente misturadas. Entretanto,
    // deve-se prestar atenção com o truncamento de valores:
    double d = 2.2;
    int i = 7;
    d = d + i;
    i = d * i;  // atribuição de expressão ao valor de "i". Cuidado com o
                // truncamento do valor resultante da expressão d*i para um int.
    print(d);
    print(i);
    // operações de modificação in place:
    print(x += y);
    print(++x);
    print(x -= y);
    print(--x);
    print(x *= y);
    print(x /= y);
    print(x %= y);

    // Inicialização:
    double d1 = 2.3;    // copy-initialization
    double d2{2.3};     // list-initialization
    double d3 = {2.3};  // list-initialization. '=' é opcional
    double d4(2.3);     // direct-initialization
    std::vector<int> v{42, 2,
                       3};  // aggregate-initialization/list-initialization
    print(v[0]);
    // int i2{7.8};  // erro de compilação. inicializador {} não permite
    // conversão
    //               // de tipos.

    // >>> Valores constantes devem ser inicializados no momento de criação dos
    // mesmos.

    // É possível utilizar 'auto' para dedução automática de tipos:
    auto b = true;          // bool
    auto ch = 'x';          // char
    auto ia = 123;          // int
    auto da = 1.2;          // double
    auto z = std::sqrt(y);  // z será qualquer tipo de retorno indicado pela
                            // função 'sqrt'. Neste caso, double.
    auto bb{true};          // bool
    print(typeid(z).name());

    // Constantes:
    // const -> Uma promessa de que o seu valor não irá mudar. Usado
    // primariamente para especificar interfaces em que sua informação pode ser
    // passada para uma função por meio de ponteiros ou referências sem temor de
    // mutabilidade. O valor de uma 'const' pode ser calculado em tempo de
    // execução. constexpr -> Significa em termos gerais 'será avaliado em tempo
    // de compilação'. Usado inicialmente para especificar constantes, para
    // permitir a inserção de informação em memória de apenas leitura e por
    // motivos de performance. O valor de uma 'constexpr' deve ser calculado em
    // tempo de compilação.
    constexpr int dmv = 16;  // dmv é uma constante nomeada
    int var = 16;            // var não é uma constante
    const double sqv =
        std::sqrt(var);  // sqv é uma constante nomeada, POSSIVELMENTE calculada
                         // em tempo de compilação.
    double sum(const std::vector<double>&);  // a função sum não irá realizar
                                             // mutações em seu argumento.
    std::vector<double> vd{1.2, 2.4, 3.4, 4.5};  // vd não é uma constante.
    const double s1 =
        sum(vd);  // ok: sum(vd) é avaliado em tempo de execução, mas a variável
                  // s1 continua sendo uma variável do tipo double e constante.
                  // constexpr double s2 = sum(vd);  // erro: sum(vd) não é uma
                  // 'constexpr'.

    // para indicar que uma expressão deva obrigatóriamente ser avaliada em
    // tempo de compilação, faz-se uso das instruções 'constexpr' e 'consteval'.

    constexpr double max1 =
        1.4 * square(16);  // ok: 1.4*square(16) é uma 'constexpr'
                           // constexpr double max2 =
                           //     1.4 * square(var);  // erro: a variável 'var'
                           //     não é uma constante, e
                           //                         // logo square(var) também
                           //                         não é uma constante.
    const double max3 =
        1.4 *
        square(var);  // ok: const max3 pode se avaliado em tempo de execução.

    // uma função constexpr pode ser utilizada para argumentos não-constantes,
    // mas neste caso não se trata de uma constexpr. Caso se deseja que tal
    // função seja executada em tempo de compilação obrigatóriamente, faz-se uso
    // da diretriz 'consteval'.
    constexpr double max4 =
        1.4 * square2(16);  // ok: a expressão 1.4*square2(16) é uma constexpr.
                            // const double max5 = 1.4 * square2(var);  // erro:
                            // var não é uma constante.

    // funções declaradas como 'constexpr' ou 'consteval' são funções puras e
    // não pode gerar efeitos colaterais. Não podem modificar variáveis
    // não-locais, mas podem conter loops e empregar suas próprias variáveis
    // locais. constexpr são necessárias em algumas regras da linguagem, tais
    // como limites de array, case label, template value arguments e constantes
    // declaradas com constexpr.

    // Ponteiros, arrays e referências:
    char va[6];        // array de 6 caracteres.
    char* p = &va[3];  // ponteiro para um caractere, que se encontra na posição
                       // [3] da array 'va'.
    char xp =
        *p;  // xp é o objeto para onde o ponteiro 'p' está apontando.
             // Empregando para-faça para imprimir os elementos de uma array:
    int v1[3] = {11, 22, 33};
    for (auto i = 0; i != 3; ++i) {
        print(v1[i]);
    }
    int v2[3] = {44, 55, 66};
    for (auto& v : v2) {
        print(v);
    }
    for (auto& v : {77, 88, 99}) {
        print(v);
    }
    // referencias se comportam de forma similar aos ponteiros, mas com
    // semântica de valor. Adicionalmente, não é possível alterar uma referência
    // para que aponte para outro objeto.

    // Quando utilizado em declarações, os operadores ('&', '*' e '[]') são
    // chamados de operadores de declaração.
    // T a[n];  // a é uma array de objetos de tipo T.
    // T* p;    // p é um ponteiro para um objeto de tipo T.
    // T & r;   // r é uma referência para um objeto de tipo T.
    // T f(A);  // f é uma função que recebe como argumento um objeto de tipo A
    // e
    //          // retorna um objeto de tipo T.

    // Ponteiro nulo:
    // use sempre o valor 'nullptr'.
    int* pv = &v1[0];
    int count{0};
    int val{22};
    if (pv) {  // if (pv != nullptr)
        print("beep");
        while (*pv) {  // while (*pv != 0)
            print("boop");
            print(*pv);
            if (*pv == val) {
                ++count;
            }
            ++pv;
        }
    }
    print(count);
    // Testes:
    char cmd = 'n';
    switch (cmd) {
        case 'u':  // fallthrough
        case 'n':
            print("primeiro bloco switch");
            break;
        case 'r':
        case 'e':
            print("segundo bloco switch");
            break;
        default:
            print("caso default.");
    }
    if (auto n = v.size()) {  // if (auto n = v.size(); n != 0)
        print("vetor 'v' não está vazio.");
        print(n);
    }
    // Atribuição:
    {
        int h = 2;
        int g = 30;
        h = g;
        print(h);
        print(g);
    }
    {
        int h = 2;
        int g = 30;
        int* ph = &h;
        int* pg = &g;
        pg = ph;
        print(*ph);
        print(*pg);
    }
    {
        int h = 2;
        int g = 30;
        int& rh = h;
        int& rg = g;
        rh = rg;
        print(h);
        print(g);
    }
    // Inicialização:
    {
        int x = 7;
        int& r{x};
        r = 72;
        print(x);
        // int& r2;  // erro: toda referência precisa ser inicializada.
    }
}
}  // namespace capitulo_1

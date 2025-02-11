#include <algorithm>
#include <any>
#include <bit>
#include <cassert>
#include <cerrno>
#include <chrono>
#include <cmath>
#include <complex>
#include <concepts>
#include <condition_variable>
#include <coroutine>
#include <filesystem>
#include <format>
#include <fstream>
#include <functional>
#include <future>
#include <generator>
#include <iomanip>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <numeric>
#include <ostream>
#include <print>
#include <queue>
#include <random>
#include <ranges>
#include <regex>
#include <set>
#include <shared_mutex>
#include <source_location>
#include <span>
#include <spanstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <syncstream>
#include <thread>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <valarray>
#include <variant>
#include <vector>

namespace capitulo_18 {
using std::cerr;
using std::cout;
using std::endl;
using std::format;
using std::shared_ptr;
using std::string;
using std::unique_ptr;
using std::vector;
using namespace std::ranges;
using namespace std::literals::string_literals;
using namespace std::literals::chrono_literals;
using namespace std::chrono;

template <typename T>
concept Printable = requires(T t) { std::cout << t; };
template <Printable... T>
void print(T&&... args) {
    (std::cout << ... << args) << std::endl;
}

template <typename T>
concept Iterable = requires(T t) {
    t.begin();
    t.end();
};
template <Iterable T>
void print(T&& iterable) {
    for (auto& x : iterable) {
        std::cout << x << " ";
    }
    std::cout << std::endl;
}

void print(std::string&& s) { std::cout << s << std::endl; }
void print(std::string_view&& s) { std::cout << s << std::endl; }

void f1(int& i) {
    int pre = i;                        // leitura do valor de 'i'.
    std::this_thread::sleep_for(50ms);  // simula alguma operação custosa
    i -= 2;                             // escrita e alteração do valor de 'i'.
    std::osyncstream osync{
        cout};  // 'osyncstream' garante que apenas uma 'thread' seja capaz de
                // realizar operações no 'cout' por vez, evitando-se a
                // ocorrência de data races na hora de mandar a mensagem para o
                // 'cout'.
    osync << format("função 'f1({})': {}", pre, i) << endl;
}

class F1 {  // 'functor'
    std::osyncstream osync{cout};

   public:
    void operator()(int& i) {
        int pre = i;                         // leitura do valor de 'i'.
        std::this_thread::sleep_for(250ms);  // simula alguma operação custosa
        i -= 2;  // escrita e alteração do valor de 'i'.
        osync << format("operador '({})' da classe 'F1': {}", pre, i) << endl;
    }
};
// as versões 'f1' e 'F1' ainda não são 'thread safe' pois podem acessar o valor
// 'i' referenciado e fazer alterações no mesmo sem controle algum, estando
// sujeitas à 'data race'. há a possibilidade de escrita de valor por uma
// 'thread', no mesmo momento em que outra está realizando operação de leitura.

void f2(int& i, std::mutex& m) {
    int pre;
    {
        std::scoped_lock lock{m};
        // ao se obter uma 'lock' de um mutex ('m.lock()'), esta thread pode
        // realizar operações diversas com os recursos obtidos sem risco
        // de sofrer 'data race' de outras threads, que terão de esperar esta
        // 'lock' ser liberada para daí poderem também realizar suas operações.
        // caso outra 'thread' tente acessar esta 'mutex' enquanto ela já
        // estiver sendo utilizada, a 'thread' é posta numa fila de espera para
        // ser processada em tempo futuro, quando a 'lock' for destravada.
        pre = i;
        std::this_thread::sleep_for(50ms);
        i -= 2;
        // a 'scoped_lock' realiza 'm.unlock()' automaticamente ao término do
        // escopo em que foi gerada.
    }
    std::osyncstream osync{cout};
    osync << format("função 'f2({})': {}", pre, i) << endl;
}

class F2 {
    std::osyncstream osync{cout};

   public:
    void operator()(int& i, std::mutex& m) {
        int pre;
        {
            std::scoped_lock lock{m};
            pre = i;
            std::this_thread::sleep_for(250ms);
            i -= 2;
        }
        osync << format("operador '({})' da classe 'F2': {}", pre, i) << endl;
    }
};

class Record {
   public:
    Record() : valor{36} {};
    Record(int i) : valor{i} {}
    int valor;
    std::mutex m{};
};

class Output {
   public:
    double valor{0};
    std::mutex m{};
};

void f3(Record& r) {
    int pre;
    {
        std::scoped_lock lock{
            r.m};  // agora, para se trabalhar com o recurso contido em 'r',
                   // deve-se obviamente obter uma 'lock' para a 'mutex' 'r.m'.
        pre = r.valor;
        r.valor -= 2;
    }
    std::osyncstream osync{cout};
    osync << format("função 'f3({})': {}", pre, r.valor) << endl;
}

class F3 {
    std::osyncstream osync{cout};

   public:
    void operator()(Record& r) {
        int pre;
        {
            std::scoped_lock lock{r.m};
            pre = r.valor;
            r.valor -= 2;
        }
        osync << format("operador '({})' da classe 'F3': {}", pre, r.valor)
              << endl;
    }
};

void f4(Record& r, Output& o) {
    std::osyncstream osync{cout};
    int pre;
    {
        std::scoped_lock lock{
            r.m, o.m};  // ao se trabalhar com mais de um recurso, e por
                        // consequẽncia 'mutexes', há o risco de gerar um 'dead
                        // lock' em que cada 'thread' consegue a lock parcial de
                        // uma mutex e fica esperando a outra thread liberar a
                        // sua respectiva lock, o que nunca acontece. a fim de
                        // se evitar esse cenário, 'scoped_lock' permite
                        // realizar a 'lock' e aquisição de mais de uma 'mutex'
                        // por vez. desta forma cada thread só adquire todos os
                        // recursos, quando todos eles estiverem disponíveis.
        pre = r.valor;
        o.valor += r.valor / 3;
        osync << format("função 'f4({})': {}", pre, o.valor) << endl;
    }
}

class F4 {
    std::osyncstream osync{cout};

   public:
    void operator()(Record& r, Output& o) {
        int pre;
        {
            std::scoped_lock lock{r.m, o.m};
            pre = r.valor;
            o.valor += r.valor % 7;
            osync << format("operador '({})' da classe 'F4': {}", pre, o.valor)
                  << endl;
        }
    }
};

// ainda, é possível encontrar na 'stl' outros tipos de 'locks', que permitem
// diversos comportamentos e são mais indicados para situações diversas.
void reader(std::shared_mutex& mx) {
    std::shared_lock lock{
        mx};  // caso se deseje que as diversas 'threads' possar realizar a
              // leitura de um determinado recurso de forma livre.
    //...
};
void writer(std::shared_mutex& mx) {
    std::unique_lock lock{
        mx};  // caso se deseje que apenas uma 'thread' por vez possa realizar a
              // operação de escrita no recurso.
    //...
};

class X {};
void use_atomic() {
    std::mutex m;
    std::atomic<bool> init_X;  // inicialmente, 'falso'.
    X x;            // variável que requer uma inicialização não trivial.
    if (!init_X) {  // verifica se alguma outra 'thread' já não tenha realizado
                    // a inicialização.
        std::lock_guard lock{m};
        if (!init_X) {  // para o caso de se existir outras 'threads' em fila de
                        // espera enquanto a inicialização está sendo feita pela
                        // primeira 'thread' a adquirir a 'lock' do recurso.
            // ...
            // ... inicialização não trivial do objeto 'X'
            // ...
            init_X =
                true;  // esta operação, por se tratar de um objeto 'atomic<>',
                       // já é repassada para todas as outras threads.
        }
    }
    // ... utilização de 'x'
}

class Message {
    //...
   public:
    Message() : mensagem{"default"} {}
    Message(string m) : mensagem{m} {}
    string mensagem;
    //...
};

template <class T>
struct Range {
    T* first;
    T* last;
};

struct task {
    void operator()() { coro.resume(); }

    struct promise_type {
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept {
            return {};
        }  // co_return
        std::suspend_always yield_value(int) { return {}; }  // co_yield
        auto get_return_object() {
            return task{handle_type::from_promise(*this)};
        }
        void return_void() {}
        void unhandled_exception() { exit(1); }
    };

    using handle_type = std::coroutine_handle<promise_type>;
    task(handle_type h) : coro{h} {}  // chamado por 'get_return_object()'
    handle_type coro;                 // coroutine handle
};

task sequencer(int start, int step = 1) {
    auto value = start;
    while (true) {
        cout << "valor: " << value << endl;  // comunica o resultado
        co_yield 0;     // adormece até que algum processo volte a reutilizar a
                        // corotina
        value += step;  // atualiza o estado interior
    }
}

task char_seq(char start) {
    auto value = start;
    while (true) {
        cout << "valor: " << value << endl;
        co_yield 0;
        ++value;
    }
}

task fibonaccer() {
    long long a = 0;
    long long b = 1;
    while (true) {
        cout << "valor: " << b << endl;  // comunica o resultado
        co_yield b;
        auto temp = a + b;
        a = b;
        b = temp;
    }
}

struct EventBase {
    virtual void operator()() = 0;
    virtual ~EventBase() {};
};

template <class Act>
struct Event : EventBase {
    Event(const string n, Act a) : name{n}, act{std::move(a)} {}
    string name;
    Act act;
    void operator()() override { act(); }
};

// o tipo de retorno deve obedecer os requisitos dados para uma corotina.
// a implementação de 'std::generator<>' é baseada em 'input range'.
std::generator<int> fib() {
    int a = 0;
    int b = 1;
    // A função deve ter pelo menos uma keyword que a caracterize como coroutina
    co_yield a;
    while (true) {
        co_yield a = std::exchange(b, a + b);
    }
};

void main() {
    auto a = []() {
        int a{44};
        std::thread t1{f1,
                       std::ref(a)};  // executa 'f1' numa 'thread' em separado
        std::thread t2{
            F1{},
            std::ref(a)};  // 'F{}()' é executado num outra 'thread' em separado
        t1.join();         // espera por 't1' finalizar
        t2.join();         // espera por 't2' finalizar
        // o método 'join()' garante que as threads serão finalizadas antes da
        // saída do escopo da função. a operação de 'join' significa 'esperar
        // até que a thread finalize'.
    };
    a();
    auto b = []() {
        std::mutex
            m{};  // mutex: 'mutual exclusion object'. um objeto chave para se
                  // realizar a troca de informações entre 'threads'.
        int b{26};
        std::jthread t1{f2, std::ref(b),
                        std::ref(m)};  // 'ref()' é necessário para informar ao
                                       // construtor que se deseja passar uma
                                       // referência de um objeto para a função.
        std::jthread t2{F2{}, std::ref(b), std::ref(m)};
        // 'jthread' é uma resposta 'RAII' para 'thread' em que realiza a 'join'
        // automaticamente no término do escopo, envitando-se assim que se
        // "esqueça" de realizar a 'join()' em algum mommento posterior. a
        // junção é realizada pelos destructors na ordem inversa de contrução.
        // neste caso, realiza-se primeiro para 't2', antes de 't1'.
        //
        // 'threads' compartilham o mesmo espaço de memória, e portanto é
        // possível realizar troca de informção. tal processo se dá por meio de
        // objetos compartilhados e 'locks'.
    };
    b();
    auto c = []() {
        // a relação entre o recurso a ser compartilhado e sua 'mutex' é baseado
        // em convenção pelo programador. para facilitar e evitar a ocorrência
        // de bugs, pode-se gerar classes e objetos que atrelem estes dois
        // conceitos num nó, a fim de deixar explícito a relação entre
        // determinado recurso e determinada 'mutex'.
        Record r{};
        std::jthread t1{f3, std::ref(r)};
        std::jthread t2{F3{}, std::ref(r)};
    };
    c();
    auto d = []() {
        Record r{10};
        Output o{};
        std::jthread t1{f4, std::ref(r), std::ref(o)};
        std::jthread t2{F4{}, std::ref(r), std::ref(o)};
    };
    d();
    auto e = []() {
        std::atomic<int> a_count{
            0};  // a variável 'atomic<>' já propaga e compartilha seu estado
                 // para outras 'threads' que podem acessar e escrever nela.
                 // entretanto, serve apenas para tipos fundamentais da
                 // linguagem.
        int count{0};
        auto f = [&a_count, &count]() {
            for (int n{10000}; n; --n) {
                ++a_count;
                ++count;
                // Note: for this example, relaxed memory order is
                // sufficient, e.g. acnt.fetch_add(1,
                // std::memory_order_relaxed);
            }
        };
        {
            std::vector<std::jthread> pool;
            for (int n = 0; n < 10; ++n) pool.emplace_back(f);
        }
        std::cout << "The atomic counter is " << a_count << endl
                  << "The non-atomic counter is " << count << endl;
    };
    e();
    auto f = []() {
        std::queue<Message>
            mqueue;  // uma fila de mensagens, a serem comunicadas
        std::condition_variable
            mcond;  // 'condition_variable' é um mecanismo que permite uma
                    // 'thread' esperar por outra, por meio de alguma condição
                    // ('evento') dependente do trabalho de alguma outra
                    // 'thread'.
        std::mutex mmutex;  // para sincronizar o acesso à 'mcond'.
        auto consumer = [&]() {
            while (true) {
                std::unique_lock lock{
                    mmutex};  // necessário utilizar 'unique_lock', pois é
                              // necessário passar o objeto 'lock' para o método
                              // 'condition_variable.wait()'. um 'scoped_lock'
                              // não pode ser movido, mas um 'unique_lock' pode.
                mcond.wait(lock, [&]() {
                    return !mqueue.empty();
                });  // libera 'mmutex' e espera. irá readiquirir 'mmutex'
                     // quando a thread voltar a ficar ativa. a 'thread' não
                     // será ativa enquanto 'mqueue' permanecer vazia.
                auto m = mqueue.front();  // aquisição da mensagem da queue.
                mqueue.pop();
                lock.unlock();  // liberação da 'mmutex'.
                // processamento da mensagem 'm' ...
            }
        };
        auto producer = [&]() {
            while (true) {
                Message m;
                // preenchimento da mensagem...
                std::scoped_lock lock{mmutex};
                mqueue.push(m);
                mcond.notify_one();  // realiza notificação
            }
        };
    };
    auto g = []() {
        // 'future' e 'promises' permitem o compartilhamento de informação entre
        // 'threads' sem que seja necessário o uso de 'locks'.
        // A idéia é: quando é necessário realizar a troca de informação, esta é
        // colocada numa 'promise'. Num tempo futuro, quando o trabalho
        // correspondente da 'promise' for concluído, é possível recuperar este
        // valor, em outra thread se desejado, por meio do objeto 'future'.
        auto f = [](std::promise<Message>&
                        pm) {  // uma tarefa: calcular e inserir o valor na
                               // 'promise'
            try {
                Message message;
                // realizar operações com o objeto 'Message'...
                message.mensagem = "mensagem bem sucedida";
                // por meio de uma 'promise' é possível passar dois valores:
                // um valor, ou uma exceção. ambos podem ser recuperados e
                // tratados por meio da respectiva 'future' em momento
                // oportuno.
                pm.set_value(message);
            } catch (...) {
                pm.set_exception(
                    std::current_exception());  // caso ocorra alguma exceção,
                                                // repassar esta para a 'thread'
                                                // que cuidará da 'future'.
            }
        };
        // auto g1 = [](std::future<Message>& fm) {
        //     //...
        //     try {
        //         Message m = fm.get();
        //         // fazer uso de 'm'...
        //     } catch (...) {
        //         //... processar o erro recebido
        //     }
        // };
        // caso 'g1' não seja responsável por lidar com o erro ocorrido, então
        // 'g1' pode se tornar mais simples:
        auto g2 = [](std::future<Message>& fm) {
            //...
            Message m = fm.get();  // busca pelo valor calculado pela respectiva
                                   // 'promise'. se tal valor não tiver sido
                                   // calculado, a 'thread' é bloqueada até que
                                   // este valor esteja disponível.
            // fazer uso de 'm'...
            print(std::string{m.mensagem});
        };
        // write-end
        std::promise<Message> pm;
        // lançar 'f' numa 'thread' própria:
        std::jthread t{f, std::ref(pm)};
        // read-end;
        std::future<Message> fm = pm.get_future();
        g2(fm);
    };
    g();
    auto h = []() {
        // 'packaged_task' é outro mecanismo para se trabalhar com 'promises' e
        // 'futures'.
        auto accum = [](std::vector<double>::iterator begin,
                        std::vector<double>::iterator end, double init) {
            return std::accumulate(begin, end, init);
        };
        std::packaged_task pt0{accum};  // 'package' uma tarefa ('accum')
        std::packaged_task pt1{accum};

        std::future<double> f0{pt0.get_future()};  // obter o 'future' de 'pt0'
        std::future<double> f1{pt1.get_future()};  // obter o 'future' de 'pt1'

        std::vector<double> v{1, 2, 3, 4, 5, 56, 6, 324};
        auto first = v.begin();

        std::jthread t1{std::move(pt0), first, first + v.size() / 2, 0.0};
        std::jthread t2{std::move(pt1), first + v.size() / 2, first + v.size(),
                        0.0};
        // 'move(pt0)' é necessário pois uma 'packaged_task' não pode ser
        // copiada, já que esta também é responsável por gerenciar recursos.
        // ...
        print(f0.get() + f1.get());
    };
    h();
    auto i = []() {
        // 'async' é mais outro mecanismo para executar funções de forma
        // assíncrona, por meio de 'threads'. permite a separação da parte da
        // 'chamada' com a parte da aquisição de valor. basta informar qual
        // atividade deve ser executada, com os respectivos argumentos, que o
        // sistema encarregará de decidir a melhor forma de execução, que poderá
        // se dar de forma assíncrona por meio de 'threads' diversas.
        // como limitação, pode-se indicar que não é possível fazer uso de
        // 'async' para situações em que seja necessário compartilhar informação
        // entre 'threads' que demandem 'locking'.
        std::vector<double> v{1243, 1234, 38, 354, 2,   35,  15, 3,  53,
                              65,   34,   6,  25,  634, 634, 63, 53, 34};
        auto accum = [](std::vector<double>::iterator begin,
                        std::vector<double>::iterator end, double init) {
            return std::accumulate(begin, end, init);
        };
        auto first = v.begin();
        auto sz = v.size();

        auto f0 = std::async(accum, first, first + sz * 1 / 4, 0.0);
        auto f1 =
            std::async(accum, first + sz * 1 / 4, first + sz * 2 / 4, 0.0);
        auto f2 =
            std::async(accum, first + sz * 2 / 4, first + sz * 3 / 4, 0.0);
        auto f3 = std::async(accum, first + sz * 3 / 4, first + sz, 0.0);
        print(f0.get() + f1.get() + f2.get() + f3.get());
    };
    i();
    auto j = []() {
        // caso seja necessário realizar a interrupção de execução de uma
        // 'thread', pode-se fazer uso de 'stop_token'. o caso a seguir, ilustra
        // uma função que busca num container um determinado valor, de forma
        // paralella, e caso alguma das 'threads' encontrem, todas as outras
        // 'threads' devem ser interrompidas, já que não é mais necessário
        // continuar a função de busca.
        std::atomic<int> result{
            -1};  // o resultado do index da busca será armazenado uma variável
                  // do tipo 'atomic<>'.
        auto find = [&result](std::stop_token token, const string* base,
                              const Range<string> r, const string target) {
            for (string* p = r.first; p != r.last && !token.stop_requested();
                 ++p) {  // '!token.stop_token' realiza o teste para verificar
                         // se alguma outra 'thread' já não tenha pedido a
                         // parada (caso tenha sido bem sucedida na busca).
                if (*p == target) {
                    result =
                        p - base;  // calcula-se o index do elemento encontrado.
                    return;
                }
            }
        };
        std::vector<string> vs{"banana", "maçã", "melão", "abóbora"};
        string key{"abóbora"};
        string* first = &vs[0];
        size_t mid = vs.size() / 2;
        {
            std::stop_source stop_s1{};
            std::jthread t1(find, stop_s1.get_token(), first,
                            Range{first, first + mid}, key);
            std::stop_source stop_s2{};
            std::jthread t2(find, stop_s2.get_token(), first,
                            Range{first + mid, first + vs.size()}, key);

            while (result == -1) {
                std::this_thread::sleep_for(10ms);
            }

            // stop_s1.request_stop();
            // stop_s2.request_stop();
            // 'jthread' automaticamente invoca 'request_stop()' e 'join()' no
            // seu destructor.
        }
        print(result);
    };
    j();
    auto k = []() {
        // corotinas são funções que são capazes de armazenar estado interno,
        // serem suspensas e retomarem seu fluxo de execução. idéia análoga à um
        // 'functor'. corotinas podem ser síncronas (o invocador deve esperar
        // pelo resultado da corotina), ou assíncrona (o invocador apenas
        // despacha a corotina para ser executada por outra 'thread' enquanto
        // vai lidar com outros processos).
        for (auto x : fib() | std::views::take(10)) {
            cout << x << " ";
        }
        cout << endl;
        // uma idéia derivada das corotinas é 'cooperative multitasking', em que
        // diversas corotinas trabalham de foma assíncrona, guiadas por um
        // 'scheduler' e uma fila de eventos para dar continuidade nos processos
        // de um programa. para tanto, é necessário: (1) diversas corotinas que
        // possam manter o estado interno entre suas chamadas. (2) uma forma de
        // polimorfismo que permite manter uma lista de eventos contendo
        // diferentes tipos de corotinas e invocá-las independentemente de seu
        // tipo e (3) um 'scheduler' que irá selecionar a próxima corotina a ser
        // executada da lista de eventos.
        std::vector<EventBase*> eventos = {
            new Event{"integers", sequencer(10)},
            new Event{"chars", char_seq('a')},
            new Event{"fibos", fibonaccer()},
        };
        // neste caso, as funções 'sequencer', 'char_seq' e 'fibonaccer' são as
        // corotinas. o que permite isso é o retorno destas funções, que é do
        // tipo 'task', capaz de manter o estado interno de cada corotina.
        vector<int> order{0, 1, 2, 0, 1, 2, 1, 2, 0, 2, 0, 1};
        for (int x : order) {
            (*eventos[x])();
        }
        for (auto p : eventos) {
            delete p;
        }
    };
    k();
};
}  // namespace capitulo_18

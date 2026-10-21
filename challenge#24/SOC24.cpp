/*!
    \file
    \brief Stack Overflow Challenge #24: a self-contained industrial poker analytics contraption.

    An intentionally disproportionate C++11 architecture whose correctness, deterministic results, and data-race freedom remain mandatory.
    A conventional solution would parse the input, classify five cards at a time, and accumulate scores. This one asynchronously decodes every card through a dedicated bounded thread pool and certifies all 2,598,960 possible five-card hands first.

    \note \li On Linux the runtime identifies the C++ standard library, but static versus dynamic linkage is a linker property not exposed by a portable C++11 macro. The current private resident measurement comes from /proc/self/smaps_rollup and is not equivalent to Windows private commit.
    \note \li Microsoft C5045 is an informational Spectre-v1 instrumentation diagnostic suppressed under MSVC /Wall /WX. Suppressing C5045 DOES NOT activate a Spectre mitigation. /Qspectre independently requests compiler-generated mitigation, and is not enabled by this source.
          \li Microsoft C4710 and C4711 only report optimizer inlining decisions, including decisions made during link-time code generation. They are suppressed under MSVC /Wall /WX without changing /O2, /Ob, /GL, /LTCG, or actual inlining decisions.
          \li Build with a C++11 standard library. The entire implementation resides in this file, without external library dependencies. On Windows the linker resolves the system-provided Psapi library for process memory telemetry.
          \li The actual problem solving duration includes task dispatch, card-to-LUT indexing, worker synchronization, and final accumulation. The input read, precalculation, certification, and report rendering are deliberately measured separately.
*/

/*!
    \par Build commands

    \par MSVC - Release x64 (C++14, dynamic CRT)
    \code{.bat}
    cl /nologo /std:c++14 /O2 /GL /DNDEBUG /EHsc /MD /Wall /WX /diagnostics:caret /FC SOC24.cpp /link /LTCG /INCREMENTAL:NO /OUT:SOC24.exe
    \endcode

    \par GCC - Release (C++11)
    \code{.sh}
    g++ -std=c++11 -O2 -DNDEBUG -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Werror -pthread SOC24.cpp -o SOC24
    \endcode

    \par Clang - Release (C++11)
    \code{.sh}
    clang++ -std=c++11 -O2 -DNDEBUG -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Werror -pthread SOC24.cpp -o SOC24
    \endcode

    \note MSVC :
        \li Commands must run from an appropriately initialized x64 Developer Command Prompt. MSVC does not provide /std:c++11. Use /std:c++14 instead.
        \li /Wall /WX remains enabled. C4710, C4711, and C5045 are selectively suppressed by the source. Windows SDK diagnostics C4668 and C5039 are suppressed only around the SDK includes.
        \li Suppressing C5045 does not activate Spectre mitigations. Add /Qspectre explicitly if required.
        \li /GL and /LTCG enable whole-program optimization and link-time code generation. /MD and /MDd select the dynamic Microsoft C runtime.
*/

/*!
    \defgroup SOC24_GLOSSARY Enterprise Acronym Reference Service
    \brief Navigable technical symbol register.
    \details Every entry links directly to its fully qualified declaration, with its acronym as the visible label.
    \note Private and static entries require EXTRACT_PRIVATE = YES and EXTRACT_STATIC = YES for complete generated documentation.
    \par Infrastructure
    - \ref ACME::ABIS "ABIS"
    - \ref ACME::ABIS::APAS "APAS"
    - \ref ACME::PDC "PDC"
    - \ref ACME::PDC::PDRC "PDRC"
    - \ref ACME::PDC::PDSC "PDSC"
    - \ref ACME::PDC::PHCC "PHCC"
    - \ref ACME::PDC::PDTC "PDTC"
    - \ref ACME::PDC::PHCTC "PHCTC"
    - \ref ACME::PDC::PHRBI "PHRBI"
    - \ref ACME::PDC::PCEBC "PCEBC"
    - \ref ACME::PDC::PCPBW "PCPBW"
    - \ref ACME::PDC::PBCE "PBCE"
    - \ref ACME::PDC::PLBMB "PLBMB"
    - \ref ACME::EFHPC "EFHPC"
    - \ref ACME::PLBG "PLBG"
    - \ref ACME::FCBMR "FCBMR"
    - \ref ACME::NCPR "NCPR"
    - \ref ACME::ALPRS "ALPRS"
    - \ref ACME::UI::RHLW "RHLW"
    - \ref ACME::UI::CPOPI "CPOPI"
    - \ref ACME::UI::CPOPI::CPRDU "CPRDU"
    - \ref ACME::UI::CPOPI::CPRLW "CPRLW"
    - \ref ACME::UI::CPOPI::CPRPL "CPRPL"
    - \ref ACME::UI::CPOPI::CPRCI "CPRCI"
    - \ref ACME::UI::CPOPI::CPRTL "CPRTL"
    - \ref ACME::IO::FCBMLE "FCBMLE"
    - \ref ACME::IO::PMORUS "PMORUS"
    - \ref ACME::IO::PMORUS::PMOSS "PMOSS"
    - \ref ACME::IO::PMORUS::PMSCO "PMSCO"
    - \ref ACME::UI::LARPRUCF "LARPRUCF"
    - \ref ACME::UI::LARPRUCF::EGPF "EGPF"
    - \ref ACME::UI::LARPRUCF::RPSUC "RPSUC"
    - \ref ACME::UI::LARPRUCF::LFNV "LFNV"
    - \ref ACME::UI::LARPRUCF::HRBC "HRBC"
    - \ref ACME::UI::LARPRUCF::HRMD "HRMD"
    - \ref ACME::UI::LARPRUCF::HRPD "HRPD"
    - \ref ACME::UI::LARPRUCF::HRIT "HRIT"
    - \ref ACME::UI::LARPRUCF::FTSRF "FTSRF"
    - \ref ACME::UI::LARPRUCF::SSSP "SSSP"
    - \ref ACME::UI::LARPRUCF::SDPOD "SDPOD"
    \par JSON and lexical processing
    - \ref ACME::JSON::GJTU "GJTU"
    - \ref ACME::JSON::GJTU::FCDHLUT "FCDHLUT"
    - \ref ACME::JSON::GJTU::FCDHLUT::CDLB "CDLB"
    - \ref ACME::JSON::GJTU::FCDHLUT::CDUB "CDUB"
    - \ref ACME::JSON::GJTU::FCDHLUT::CDCE "CDCE"
    - \ref ACME::JSON::GJTU::FCDHLUT::FCDHVA "FCDHVA"
    - \ref ACME::JSON::GJTU::FCDHLUT::TSSLR "TSSLR"
    - \ref ACME::JSON::GJTU::HCDVAD "HCDVAD"
    - \ref ACME::JSON::GJTU::UCSTUAE "UCSTUAE"
    - \ref ACME::JSON::GJTU::FCUHPT "FCUHPT"
    - \ref ACME::JSON::GJTU::GJSEDE "GJSEDE"
    - \ref ACME::JSON::CPDA "CPDA"
    - \ref ACME::JSON::CPDA::CPPA "CPPA"
    - \ref ACME::JSON::CPDA::CPPA::CLCUNNR "CLCUNNR"
    - \ref ACME::JSON::CPDA::CPPA::CPSDV "CPSDV"
    - \ref ACME::JSON::CPDA::TLDA "TLDA"
    - \ref ACME::JSON::CPDA::TLDA::GEJT2LD "GEJT2LD"
    - \ref ACME::JSON::CPDA::TLDA::GEJT2LD::TJCSDE "TJCSDE"
    - \ref ACME::JSON::CPDA::TLDA::LCDA "LCDA"
    - \ref ACME::JSON::JGSP "JGSP"
    - \ref ACME::JSON::JGSP::TLCFEA "TLCFEA"
    - \ref ACME::JSON::JGSP::GJSSLDES "GJSSLDES"
    - \ref ACME::JSON::JGSP::GJSSLDES::ASBSDP "ASBSDP"
    - \ref ACME::JSON::JGSP::GJSSLDES::ASCRR "ASCRR"
    - \ref ACME::JSON::JGSP::GJSSLDES::CLCCQ "CLCCQ"
    - \ref ACME::JSON::JGSP::GJSSLDES::DLCCQ "DLCCQ"
    - \ref ACME::JSON::JGSP::GJSSLDES::SBPCQ "SBPCQ"
    - \ref ACME::JSON::JGSP::GJSSLDES::SSCSQ "SSCSQ"
    - \ref ACME::JSON::JGSP::GJSSLDES::SJCKC "SJCKC"
    - \ref ACME::JSON::JGSP::GJSSLDES::JGPS "JGPS"
    - \ref ACME::JSON::JGSP::GJSSLDES::JGSF "JGSF"
    - \ref ACME::JSON::JGSP::GJSSLDES::JSWRP "JSWRP"
    - \ref ACME::JSON::JGSP::GJSSLDES::JRECV "JRECV"
    - \ref ACME::JSON::JGSP::GJSSLDES::JSLP "JSLP"
    - \ref ACME::JSON::JGSP::GJSSLDES::JNLP "JNLP"
    - \ref ACME::JSON::JGSP::GJSSLDES::JVDP "JVDP"
    - \ref ACME::JSON::JGSP::GJSSLDES::JCFP "JCFP"
    - \ref ACME::JSON::JGSP::GJSSLDES::TLATD "TLATD"
    - \ref ACME::JSON::JGSP::GJSSLDES::JGSDE "JGSDE"
    \par Concurrency and poker
    - \ref ACME::AECS "AECS"
    - \ref ACME::AECS::PGTPE "PGTPE"
    - \ref ACME::AECS::PGTPE::PGTSS "PGTSS"
    - \ref ACME::AECS::PGTPE::PTSS "PTSS"
    - \ref ACME::AECS::PGTPE::ATJSA "ATJSA"
    - \ref ACME::AECS::PGTPE::ATJFA "ATJFA"
    - \ref ACME::AECS::PGTPE::WQCED "WQCED"
    - \ref ACME::PEA "PEA"
    - \ref ACME::PEA::CTCCGPLUT "CTCCGPLUT"
    - \ref ACME::PEA::CTCCGPLUT::PEVSA "PEVSA"
    - \ref ACME::PEA::CTCCGPLUT::PEVRA "PEVRA"
    - \ref ACME::PEA::CTCCGPLUT::PBSA "PBSA"
    - \ref ACME::PEA::DBBIRL "DBBIRL"
    - \ref ACME::PEA::LSBIQ "LSBIQ"
    - \ref ACME::PEA::MSBIQ "MSBIQ"
    - \ref ACME::PEA::BCTRA "BCTRA"
    - \ref ACME::PEA::CBIHMU "CBIHMU"
    - \ref ACME::PEA::CBHMRI "CBHMRI"
    - \ref ACME::PEA::GCSNME "GCSNME"
    - \ref ACME::PEA::FCPHCE "FCPHCE"
    - \ref ACME::PEA::FGEPCEC "FGEPCEC"
    - \ref ACME::PEA::FGEPCEC::EPCPO "EPCPO"
*/

// Architecture detection.
#if defined(__clang__)
# define SOC24_COMPILER_NAME "Clang"
# define SOC24_COMPILER_VERSION (__clang_major__ * 100 + __clang_minor__)
#elif defined(_MSC_VER)
# define SOC24_COMPILER_NAME "MSVC"
# define SOC24_COMPILER_VERSION _MSC_VER
#elif defined(__GNUC__)
# define SOC24_COMPILER_NAME "GCC"
# define SOC24_COMPILER_VERSION (__GNUC__ * 100 + __GNUC_MINOR__)
#else
# error Unsupported compiler.
#endif
#if defined(_MSVC_LANG)
# define SOC24_LANGUAGE_VERSION _MSVC_LANG
#else
# define SOC24_LANGUAGE_VERSION __cplusplus
#endif
#if SOC24_LANGUAGE_VERSION < 201103L
# error A C++11-compliant compiler is required.
#endif
#if defined(_MSC_VER)
// C5045 reports possible /Qspectre instrumentation locations, not an enabled mitigation.
// /Qspectre must be enabled independently to request Spectre-v1 instrumentation.
// C4710: not inlined, C4711: automatically inlined. Informational even under /GL /LTCG.
// Disable both before parsing the standard-library headers to cover optimized linker output.
# pragma warning(disable: 4710 4711 5045)
#endif

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdlib>
#include <cstdint>
#include <exception>
#include <fstream>
#include <functional>
#include <future>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <list>
#include <locale>
#include <limits>
#include <memory>
#include <numeric>
#include <mutex>
#include <queue>
#include <random>
#include <ratio>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

#if defined(_WIN32)
# if defined(_MSC_VER)
// Windows SDK 10+ emits C5039 in winbase.h and C4668 in winioctl.h
// under /Wall /WX. Neither diagnostic originates in this translation unit.
// Scope these exemptions strictly to SDK headers. Project warnings stay enabled.
#  pragma warning(push)
#  pragma warning(disable: 4668 5039)
# endif
# ifndef NOMINMAX
#  define NOMINMAX
# endif
# include <windows.h>
# include <psapi.h>
# if defined(_MSC_VER)
#  pragma warning(pop)
#  pragma comment(lib, "psapi.lib")
# endif
#endif

#if defined(__linux__)
# include <unistd.h>
#endif

namespace ACME {

    /*!
        \brief Architecture-Based Interstitial Spacing.
        The dedicated MSVC /Wall compliance budget requires every byte of otherwise compiler-provided padding to receive its own job title.
        On x86 the natural pointer-sized alignment is four bytes, on x64 eight.
        This extravagance is deliberate, not a request to pack structures.
    */
    namespace ABIS {
        //! Architecture-Parameterized Alignment Span.
        #if defined(_M_X64) || defined(__x86_64__) || defined(__amd64__)
        static const std::size_t APAS = 8u;
        #elif defined(_M_IX86) || defined(__i386__)
        static const std::size_t APAS = 4u;
        #else
        static const std::size_t APAS = alignof(std::size_t);
        #endif
        static_assert(APAS == sizeof(void*), "Unsupported pointer alignment model");
        static_assert(alignof(std::size_t) == APAS, "Unexpected word alignment model");
    }

    namespace PDC {
        //! Poker Deck Rank Cardinality.
        constexpr std::size_t PDRC = 13;
        //! Poker Deck Suit Cardinality.
        constexpr std::size_t PDSC = 4;
        //! Poker Hand Card Cardinality.
        constexpr std::size_t PHCC = 5;
        //! Poker Deck Total Cardinality.
        constexpr std::size_t PDTC = PDRC * PDSC;
        //! Poker Hand Category Cardinality.
        constexpr std::size_t PHCTC = 10;
        //! Poker Highest-Ranking Royal Straight Bit Index.
        constexpr std::size_t PHRBI = PDRC - PHCC;
        //! Poker Category Encoding Bit Count.
        constexpr std::size_t PCEBC(std::size_t categories) { return categories <= 1u ? 0u : 1u + PCEBC((categories + 1u) / 2u); }
        //! Poker Category Packed Bit Width.
        constexpr std::size_t PCPBW = PCEBC(PHCTC);
        //! Poker Binomial Coefficient Evaluation, expressed recursively for C++11 constexpr.
        constexpr std::size_t PBCE(std::size_t elements, std::size_t selected, std::size_t step = 1u, std::size_t value = 1u) { return step > selected ? value : PBCE(elements, selected, step + 1u, value * (elements - selected + step) / step); }
        //! Packed Lookup Block Memory Budget. Four kibibytes are a scheduling heuristic, not an assertion about the operating system page size.
        constexpr std::size_t PLBMB = std::size_t{1} << 12u;
    }

    //! Exhaustive Five-card Hand Population Cardinality, computed as the binomial coefficient C(deck size, hand size).
    static constexpr std::size_t EFHPC = PDC::PBCE(PDC::PDTC, PDC::PHCC);
    //! Packed Lookup Block Granularity, derived from the chosen byte budget and category bit width.
    static constexpr std::size_t PLBG = PDC::PLBMB * std::numeric_limits<std::uint8_t>::digits / PDC::PCPBW;
    static_assert(PDC::PCPBW != 0u && PDC::PCPBW < std::numeric_limits<std::uint8_t>::digits, "Packed category width must fit within a storage byte");
    static_assert(PDC::PHCTC <= (std::size_t{1} << PDC::PCPBW), "Insufficient packed category bits");
    static_assert(PLBG % (std::numeric_limits<std::uint8_t>::digits / PDC::PCPBW) == 0u, "Work blocks must begin on packed-byte boundaries");

    typedef std::uint64_t FCBMR;

    /*!
        \brief Normalized Card Payload Record.
        \note Two unsigned bytes would also work without requiring an acronym registry.
    */
    struct NCPR {
        std::uint8_t rank;
        std::uint8_t suit;
    };

    /*!
        \brief Asynchronous Leaf Processing Result Slot.
        An exception pointer travels alongside the result of the worker thread.
        \note Synchronous decoding would make this wrapper unnecessary.
    */
    template<typename Value>
    struct ALPRS {
        static_assert(std::is_trivially_copyable<Value>::value, "The asynchronous leaf slot requires trivially copyable values");
        static_assert(std::is_default_constructible<Value>::value, "The asynchronous leaf slot requires default construction");
        static_assert(alignof(Value) <= ABIS::APAS, "The asynchronous leaf slot requires pointer-compatible alignment");
        Value card;
        std::uint8_t alignmentPadding[ABIS::APAS - sizeof(Value) % ABIS::APAS]; //!< Architecture-dependent explicit padding before the exception pointer.
        std::exception_ptr error;
        ALPRS() : card(), alignmentPadding(), error() {}
    };
    static_assert(sizeof(NCPR) == 2, "Unexpected card representation");
    static_assert(sizeof(bool) == 1 && sizeof(std::atomic<bool>) == 1, "Unexpected boolean storage model");
    static_assert(alignof(std::exception_ptr) == ABIS::APAS, "Unexpected exception-pointer alignment");
    static_assert(sizeof(ALPRS<NCPR>) == sizeof(NCPR) + (ABIS::APAS - sizeof(NCPR) % ABIS::APAS) + sizeof(std::exception_ptr), "Unexpected leaf-slot padding");

    namespace UI {

        //! Report Horizontal Line Width, chosen as nine conventional eight-character terminal tab stops.
        constexpr std::size_t RHLW = 9u * 8u;

        /*!
            \brief Corporate Progress and Operational Presentation Indicator.
            The economical alternative is to run the calculation and print its answer.
            A progress indicator, however, provides the main thread with a respectable occupation while additional threads parse five-card hands.
        */
        struct CPOPI {
            //! Corporate Progress Rendering Display Update.
            static void CPRDU(const std::string& phase, std::size_t CLCCQ, std::size_t total, const std::string& extra = "") {
                const auto width = RHLW / 2u;
                const std::size_t units = total ? (width * std::min(CLCCQ, total) / total) : 0;
                std::ostringstream stream;
                stream << std::left << std::setw(23) << phase << '[' << std::string(units, '#') << std::string(width - units, '.') << "] " << std::right << std::setw(3) << (total ? 100 * std::min(CLCCQ, total) / total : 0) << "% " << extra;
                const std::string line = stream.str();
                if (!CPRCI()) {
                    CPRPL() = line;
                    return;
                }
                const auto preceding = CPRLW();
                std::cout << '\r' << line << std::string(preceding > line.size() ? preceding - line.size() : 0u, ' ') << std::flush;
                CPRLW() = line.size();
            }

            //! Corporate Progress Rendering Terminal Linebreak.
            static void CPRTL() {
                if (CPRCI())
                    std::cout << '\n';
                else
                    std::cout << CPRPL() << '\n';
                CPRPL().clear();
                CPRLW() = 0u;
            }

        private:
            //! Corporate Progress Rendered Line Width State.
            static std::size_t& CPRLW() {
                static std::size_t renderedWidth = 0u;
                return renderedWidth;
            }

            //! Corporate Progress Retained Presentation Line.
            static std::string& CPRPL() {
                static std::string line;
                return line;
            }

            //! Corporate Progress Rendering Console Interactivity.
            static bool CPRCI() {
        #if defined(_WIN32)
                DWORD mode = 0;
                return GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &mode) != 0;
        #elif defined(__linux__)
                return ::isatty(STDOUT_FILENO) != 0;
        #else
                return false;
        #endif
            }
        };

    }

    namespace IO {

        /*!
            \brief Full-Content Byte Materialization and Loading Engine.
            \param[in] path Input file path.
            \return Complete file bytes as a string.
            Streaming the input would reduce peak memory consumption; bulk loading makes the scanner's asynchronous lifecycle feel more enterprise-ready.
        */
        static std::string FCBMLE(const char* path) {
            std::ifstream input(path, std::ios::in | std::ios::binary);
            if (!input)
                throw std::runtime_error(std::string("Cannot open ") + path);
            std::string result((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
            if (input.bad())
                throw std::runtime_error("I/O failure while reading JSON");
            return result;
        }

    }

    //! Asynchronous Enterprise Concurrency Services.
    namespace AECS {

        /*!
            \brief Production-Grade Thread-Pool Executor.
            \details Multiple independent instances can be deployed with independently bounded dispatch queues. Oversubscribed worker populations are deliberately encouraged for operating-system scheduling exercise.
            \note Queuing is bounded so arbitrarily long input cannot materialize an unbounded number of pending callable wrappers.
        */
        class PGTPE {

        public:
            //! Production-Grade Thread-Pool Executor.
            explicit PGTPE(std::uint32_t count) : queueCapacity_(std::max(std::size_t{1}, static_cast<std::size_t>(count)) * 8u), submitted_(0), started_(0), completed_(0), queuedPeak_(0), busy_(0), busyPeak_(0), throttled_(0), totalQueueWait_(0), maximumQueueWait_(0), totalWork_(0), maximumWork_(0), stopping_(false), terminationPadding_() {
                if (!count)
                    count = 1;
                try {
                    for (auto i = decltype(count){}; i < count; ++i)
                        workers_.emplace_back([this]() { WQCED(); });
                } catch (...) {
                    {
                        std::lock_guard<std::mutex> lock(mutex_);
                        stopping_ = true;
                    }
                    cv_.notify_all();
                    std::for_each(workers_.begin(), workers_.end(), [](std::thread& worker) { worker.join(); });
                    throw;
                }
            }

            //! Asynchronous Task Job Submission Adapter.
            template<typename Callable>
            auto ATJSA(Callable&& job) -> std::future<typename std::result_of<typename std::decay<Callable>::type&()>::type> {
                typedef typename std::decay<Callable>::type Job;
                typedef typename std::result_of<Job&()>::type Result;
                static_assert(std::is_move_constructible<Job>::value, "Submitted tasks must be move-constructible");
                static_assert(std::is_destructible<Job>::value, "Submitted tasks must be destructible");
                std::shared_ptr<std::packaged_task<Result()> > task(new std::packaged_task<Result()>(std::forward<Callable>(job)));
                auto result = task->get_future();
                ATJFA([task]() { (*task)(); });
                return result;
            }

            //! Asynchronous Task Job Fire-and-Forget Adapter.
            template<typename Callable>
            void ATJFA(Callable&& job) {
                typedef typename std::decay<Callable>::type Job;
                static_assert(std::is_copy_constructible<Job>::value, "Fire-and-forget tasks must be copy-constructible");
                static_assert(std::is_same<void, typename std::result_of<Job&()>::type>::value, "Fire-and-forget tasks must return void");
                std::function<void()> task(std::forward<Callable>(job));
                {
                    std::unique_lock<std::mutex> lock(mutex_);
                    if (queue_.size() == queueCapacity_)
                        ++throttled_;
                    capacityCv_.wait(lock, [this]() { return stopping_ || queue_.size() < queueCapacity_; });
                    if (stopping_)
                        throw std::runtime_error("Submit after thread-pool shutdown");
                    queue_.emplace(std::chrono::steady_clock::now(), std::move(task));
                    ++submitted_;
                    queuedPeak_ = std::max(queuedPeak_, queue_.size());
                }
                cv_.notify_one();
            }

            /*!
                \brief Pool Generic Telemetry State Snapshot.
                \details Fields are coherent at capture time, not individually sampled racing counters.
            */
            struct PGTSS {
                std::size_t workers;
                std::size_t capacity;
                std::size_t queued;
                std::size_t queuedPeak;
                std::size_t active;
                std::size_t activePeak;
                std::size_t submitted;
                std::size_t started;
                std::size_t completed;
                std::size_t throttled;
                std::chrono::nanoseconds queueWaitTotal;
                std::chrono::nanoseconds queueWaitMaximum;
                std::chrono::nanoseconds workerTimeTotal;
                std::chrono::nanoseconds workerTimeMaximum;
            };

            //! Pool Telemetry State Snapshot.
            PGTSS PTSS() {
                std::lock_guard<std::mutex> lock(mutex_);
                return PGTSS{workers_.size(), queueCapacity_, queue_.size(), queuedPeak_, busy_, busyPeak_, submitted_, started_, completed_, throttled_, totalQueueWait_, maximumQueueWait_, totalWork_, maximumWork_};
            }

            PGTPE(const PGTPE&) = delete;
            PGTPE& operator=(const PGTPE&) = delete;
            PGTPE(PGTPE&&) = delete;
            PGTPE& operator=(PGTPE&&) = delete;

            ~PGTPE() {
                {
                    std::lock_guard<std::mutex> lock(mutex_);
                    stopping_ = true;
                }
                cv_.notify_all();
                capacityCv_.notify_all();
                std::for_each(workers_.begin(), workers_.end(), [](std::thread& worker) { worker.join(); });
            }

        private:
            //! Worker Queue Consumption Execution Dispatcher.
            void WQCED() {
                for (;;) {
                    std::function<void()> job;
                    {
                        std::unique_lock<std::mutex> lock(mutex_);
                        cv_.wait(lock, [this]() { return stopping_ || !queue_.empty(); });
                        if (stopping_ && queue_.empty())
                            return;
                        const auto wait = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - queue_.front().first);
                        totalQueueWait_ += wait;
                        maximumQueueWait_ = std::max(maximumQueueWait_, wait);
                        job = std::move(queue_.front().second);
                        queue_.pop();
                        ++started_;
                        ++busy_;
                        busyPeak_ = std::max(busyPeak_, busy_);
                    }
                    capacityCv_.notify_one();
                    const auto workStarted = std::chrono::steady_clock::now();
                    job();
                    const auto workTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - workStarted);
                    {
                        std::lock_guard<std::mutex> lock(mutex_);
                        totalWork_ += workTime;
                        maximumWork_ = std::max(maximumWork_, workTime);
                        --busy_;
                        ++completed_;
                    }
                }
            }
            std::vector<std::thread> workers_;
            std::queue<std::pair<std::chrono::steady_clock::time_point, std::function<void()> > > queue_;
            std::mutex mutex_;
            std::condition_variable cv_;
            std::condition_variable capacityCv_;
            const std::size_t queueCapacity_;
            std::size_t submitted_;
            std::size_t started_;
            std::size_t completed_;
            std::size_t queuedPeak_;
            std::size_t busy_;
            std::size_t busyPeak_;
            std::size_t throttled_;
            std::chrono::nanoseconds totalQueueWait_;
            std::chrono::nanoseconds maximumQueueWait_;
            std::chrono::nanoseconds totalWork_;
            std::chrono::nanoseconds maximumWork_;
            bool stopping_;
            std::uint8_t terminationPadding_[ABIS::APAS - sizeof(bool)]; //!< Deliberately materialized worker-pool tail padding.
            static_assert(sizeof(terminationPadding_) + sizeof(bool) == ABIS::APAS, "Unexpected pool padding policy");
        };

    }

    namespace IO {

        //! Process Memory Observation and Resource Utilization Survey.
        struct PMORUS {
            //! Process Memory Observation State Snapshot.
            struct PMOSS {
                std::uint64_t residentCurrent;
                std::uint64_t residentPeak;
                std::uint64_t privateCurrent;
                std::uint64_t privatePeak;
                std::uint64_t virtualCurrent;
                std::uint64_t virtualPeak;
            };

            //! Process Memory Snapshot Collection.
            static PMOSS PMSCO() {
                PMOSS result = {};
        #if defined(_WIN32)
                PROCESS_MEMORY_COUNTERS_EX counters = {};
                counters.cb = static_cast<DWORD>(sizeof(counters));
                if (GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&counters), static_cast<DWORD>(sizeof(counters)))) {
                    result.residentCurrent = static_cast<std::uint64_t>(counters.WorkingSetSize);
                    result.residentPeak = static_cast<std::uint64_t>(counters.PeakWorkingSetSize);
                    result.privateCurrent = static_cast<std::uint64_t>(counters.PrivateUsage);
                    result.privatePeak = static_cast<std::uint64_t>(counters.PeakPagefileUsage);
                }
        #elif defined(__linux__)
                std::ifstream input("/proc/self/status");
                std::string line;
                while (std::getline(input, line)) {
                    std::istringstream fields(line);
                    std::string name;
                    std::uint64_t kilobytes = 0;
                    if (!(fields >> name >> kilobytes))
                        continue;
                    if (name == "VmRSS:") result.residentCurrent = kilobytes * 1024u;
                    else if (name == "VmHWM:") result.residentPeak = kilobytes * 1024u;
                    else if (name == "VmSize:") result.virtualCurrent = kilobytes * 1024u;
                    else if (name == "VmPeak:") result.virtualPeak = kilobytes * 1024u;
                }
                std::ifstream rollup("/proc/self/smaps_rollup");
                while (std::getline(rollup, line)) {
                    std::istringstream fields(line);
                    std::string name;
                    std::uint64_t kilobytes = 0;
                    if (!(fields >> name >> kilobytes))
                        continue;
                    if (name == "Private_Clean:" || name == "Private_Dirty:" || name == "Private_Hugetlb:")
                        result.privateCurrent += kilobytes * 1024u;
                }
        #endif
                return result;
            }
        };

    }

    namespace UI {

        //! Locale-Aware Ratio-Parameterized Reporting and Unit Conversion Facility.
        class LARPRUCF {
            //! Explicit Grouping Punctuation Facet.
            class EGPF : public std::numpunct<char> {

            public:
                explicit EGPF(char groupingCharacter) : separator_(static_cast<unsigned char>(groupingCharacter)) {}

                EGPF(const EGPF&) = delete;
                EGPF& operator=(const EGPF&) = delete;
                EGPF(EGPF&&) = delete;
                EGPF& operator=(EGPF&&) = delete;

            protected:
                char do_thousands_sep() const override { return static_cast<char>(separator_); }

                std::string do_grouping() const override { return "\3"; }

            private:
                std::size_t separator_;
            };
            std::locale locale_;

        public:
            LARPRUCF() : locale_(std::locale::classic()) {
                try { locale_ = std::locale(""); } catch (const std::runtime_error&) {}
                const auto& punctuation = std::use_facet<std::numpunct<char> >(locale_);
                const auto nativeSeparator = punctuation.thousands_sep();
                const auto separator = punctuation.grouping().empty() ? ',' : static_cast<unsigned char>(nativeSeparator) >= 128u ? ' ' : nativeSeparator;
                locale_ = std::locale(locale_, new EGPF(separator));
            }

            //! Ratio-Parameterized Scalar Unit Conversion.
            template<typename SourceRatio, typename DestinationRatio, typename Number>
            static long double RPSUC(Number amount) {
                static_assert(std::is_arithmetic<Number>::value, "Only arithmetic magnitudes are convertible");
                typedef typename std::ratio_divide<SourceRatio, DestinationRatio>::type Factor;
                return static_cast<long double>(amount) * static_cast<long double>(Factor::num) / static_cast<long double>(Factor::den);
            }

            //! Locale-Formatted Numeric Value.
            template<typename Number>
            std::string LFNV(Number value, int decimals = 0) const {
                std::ostringstream stream;
                stream.imbue(locale_);
                stream << std::fixed << std::setprecision(decimals) << value;
                return stream.str();
            }

            //! Human-Readable Byte Count with Adaptive Binary-Multiple Equivalent.
            std::string HRBC(std::uint64_t bytes) const {
                const auto exact = LFNV(bytes) + " B";
                if (bytes < 1024u)
                    return exact;
                static const char* const units[] = {"B", "KiB", "MiB", "GiB", "TiB", "PiB", "EiB"};
                auto value = static_cast<long double>(bytes);
                auto unit = std::size_t{};
                do {
                    value /= 1024.0L;
                    ++unit;
                } while (value >= 999.9995L && unit + 1u < sizeof(units) / sizeof(units[0]));
                return exact + " (" + LFNV(value, 3) + ' ' + units[unit] + ')';
            }

            //! Human-Readable Magnitude-Driven Duration.
            std::string HRMD(std::chrono::steady_clock::duration elapsed) const {
                const auto seconds = std::chrono::duration<long double>(elapsed).count();
                const auto nanoseconds = RPSUC<std::ratio<1>, std::nano>(seconds);
                if (nanoseconds < 999.9995L)
                    return LFNV(nanoseconds, 3) + " ns";
                const auto microseconds = RPSUC<std::ratio<1>, std::micro>(seconds);
                if (microseconds < 999.9995L)
                    return LFNV(microseconds, 3) + u8" \u00B5s";
                const auto milliseconds = RPSUC<std::ratio<1>, std::milli>(seconds);
                if (milliseconds < 999.9995L)
                    return LFNV(milliseconds, 3) + " ms";
                if (seconds < 60.0L)
                    return LFNV(seconds, 3) + " s";
                const auto minutes = seconds / 60.0L;
                if (minutes < 60.0L)
                    return LFNV(minutes, 3) + " min";
                const auto hours = minutes / 60.0L;
                if (hours < 24.0L)
                    return LFNV(hours, 3) + " h";
                return LFNV(hours / 24.0L, 3) + " d";
            }

            //! Human-Readable Precision Duration, Reusing Adaptive Unit Selection.
            std::string HRPD(std::chrono::steady_clock::duration elapsed) const {
                return HRMD(elapsed);
            }

            //! Human-Readable Item-Per-Second Throughput with Adaptive SI Prefix.
            std::string HRIT(std::size_t items, std::chrono::steady_clock::duration elapsed, const std::string& noun) const {
                const auto seconds = std::chrono::duration<long double>(elapsed).count();
                if (seconds <= 0.0L)
                    return "unmeasurable " + noun + "/s";
                static const char* const prefixes[] = {"", "k", "M", "G", "T", "P", "E"};
                auto value = static_cast<long double>(items) / seconds;
                auto prefix = std::size_t{};
                while (value >= 999.9995L && prefix + 1u < sizeof(prefixes) / sizeof(prefixes[0])) {
                    value /= 1000.0L;
                    ++prefix;
                }
                return LFNV(value, 3) + ' ' + prefixes[prefix] + noun + "/s";
            }

            //! Fixed-Tab-Stop Telemetry Report Field.
            void FTSRF(const std::string& label, const std::string& value) const {
                const std::string left = "    " + label + ':';
                std::cout << left;
                auto column = left.size();
                do { std::cout << '\t'; column = ((column / 8u) + 1u) * 8u; } while (column < 40u);
                std::cout << value << '\n';
            }

            //! Standardized Section Separator.
            void SSSP() const {
                std::cout << '\n' << std::string(RHLW, '=') << "\n\n";
            }

            //! Snapshot Detailed Pool Operational Diagnostics.
            void SDPOD(const std::string& name, const AECS::PGTPE::PGTSS& state) const {
                std::cout << "  " << name << "\n";
                FTSRF("Workers allocated", LFNV(state.workers) + " threads");
                FTSRF("Peak active workers", LFNV(state.activePeak) + " threads");
                FTSRF("Tasks submitted", LFNV(state.submitted) + " tasks");
                FTSRF("Tasks started", LFNV(state.started) + " tasks");
                FTSRF("Tasks completed", LFNV(state.completed) + " tasks");
                FTSRF("FIFO capacity", LFNV(state.capacity) + " tasks");
                FTSRF("FIFO peak occupancy", LFNV(state.queuedPeak) + " tasks");
                FTSRF("FIFO final occupancy", LFNV(state.queued) + " tasks");
                FTSRF("Active workers at snapshot", LFNV(state.active) + " threads");
                FTSRF("Throttled submissions", LFNV(state.throttled) + " events");
                FTSRF("Total FIFO waiting time", HRMD(state.queueWaitTotal));
                FTSRF("Mean FIFO waiting time", state.started ? HRPD(state.queueWaitTotal / static_cast<std::int64_t>(state.started)) : "N/A");
                FTSRF("Maximum FIFO waiting time", HRPD(state.queueWaitMaximum));
                FTSRF("Aggregate worker execution", HRMD(state.workerTimeTotal));
                FTSRF("Mean task execution time", state.completed ? HRPD(state.workerTimeTotal / static_cast<std::int64_t>(state.completed)) : "N/A");
                FTSRF("Maximum task execution", HRPD(state.workerTimeMaximum));
                std::cout << '\n';
            }
        };

    }

    namespace JSON {
        //! Generic JSON Toolbox Utilities.
        namespace GJTU {

            /*!
                \brief Full Character Domain Hexadecimal Lookup Table.
                For every representable plain char, including negative values on systems with signed char, a stable std::array entry stores either its hex digit value (0..15) or -1. No locale, no repeated comparisons, no case folding.
                The table size derives from numeric_limits<char>::min()/max(), rather than assuming a particular signedness of plain char. The index is shifted by the minimum representable value to avoid negative array subscripts.
                \note A three-branch decoder would be more economical for four hexadecimal digits per Unicode escape. The lookup table is intentionally optimized without any regard for how often this code is actually executed.
            */
            class FCDHLUT {

            public:
                /*!
                    \brief CDLB/CDUB/CDCE: Character Domain Lower/Upper Bounds and Cardinality Enumeration.
                    These entries make an entire char-domain LUT indexable regardless of signedness.
                */
                enum {
                    CDLB = std::numeric_limits<char>::min(),
                    CDUB = std::numeric_limits<char>::max(),
                    CDCE = CDUB - CDLB + 1
                };

                //! Full Character Domain Hexadecimal Value Acquisition.
                static int FCDHVA(char input) noexcept {
                    const std::size_t index = static_cast<std::size_t>(static_cast<int>(input) - CDLB);
                    return static_cast<int>(TSSLR()[index]);
                }

            private:
                //! Thread-Safe Singleton Lookup Repository.
                static const std::array<std::int8_t, CDCE>& TSSLR() {
                    static const std::array<std::int8_t, CDCE> entries = []() {
                        std::array<std::int8_t, CDCE> table = {};
                        table.fill(static_cast<std::int8_t>(-1));
                        for (auto digit = std::size_t{}; digit != 10u; ++digit)
                            table[static_cast<std::size_t>('0' - CDLB) + digit] = static_cast<std::int8_t>(digit);
                        for (auto digit = std::size_t{}; digit != 6u; ++digit) {
                            table[static_cast<std::size_t>("abcdef"[digit] - CDLB)] = static_cast<std::int8_t>(digit + 10);
                            table[static_cast<std::size_t>("ABCDEF"[digit] - CDLB)] = static_cast<std::int8_t>(digit + 10);
                        }
                        return table;
                    }();
                    return entries;
                }
            };

            /*!
                \brief Hexadecimal Character Digit Value Acquisition Delegate.
                \param[in] character One character from a JSON Unicode escape.
                \return Hexadecimal digit 0..15, or -1 if invalid.
            */
            static int HCDVAD(char character) noexcept {
                return FCDHLUT::FCDHVA(character);
            }

            //! Unicode Codepoint Serialization to UTF-8 Appending Engine.
            static void UCSTUAE(std::string& out, std::uint32_t cp) {
                if (cp <= 0x7F)
                    out += static_cast<char>(cp);
                else if (cp <= 0x7FF) {
                    out += static_cast<char>(0xC0u | (cp >> 6));
                    out += static_cast<char>(0x80u | (cp & 0x3Fu));
                } else if (cp <= 0xFFFF) {
                    out += static_cast<char>(0xE0u | (cp >> 12));
                    out += static_cast<char>(0x80u | ((cp >> 6) & 0x3Fu));
                    out += static_cast<char>(0x80u | (cp & 0x3Fu));
                } else {
                    out += static_cast<char>(0xF0u | (cp >> 18));
                    out += static_cast<char>(0x80u | ((cp >> 12) & 0x3Fu));
                    out += static_cast<char>(0x80u | ((cp >> 6) & 0x3Fu));
                    out += static_cast<char>(0x80u | (cp & 0x3Fu));
                }
            }

            /*!
                \brief Four-Character Unicode Hexadecimal Parsing Transaction.
                \param[in] text Source JSON text.
                \param[in,out] pos In/out cursor positioned at the first hexadecimal character.
                \return Decoded 16-bit code unit.
                A reasonable system could use a small arithmetic helper here. the application uses a full-domain LUT initialized through a singleton, because it can.
            */
            static std::uint32_t FCUHPT(const std::string& text, std::size_t& pos) {
                if (pos > text.size() || text.size() - pos < 4)
                    throw std::runtime_error("Truncated Unicode escape");
                std::uint32_t cp = {};
                for (std::size_t i = {}; i != sizeof(std::uint16_t) * 2u; ++i) {
                    const auto v = HCDVAD(text[pos++]);
                    if (v < 0)
                        throw std::runtime_error("Invalid Unicode escape");
                    cp = (cp << 4) | static_cast<std::uint32_t>(v);
                }
                return cp;
            }

            //! Four-Character Unicode Hexadecimal Parsing Transaction over an immutable character span.
            static std::uint32_t FCUHPT(const char*& cursor, const char* last) {
                if (last - cursor < 4)
                    throw std::runtime_error("Truncated Unicode escape");
                std::uint32_t cp = {};
                for (auto i = std::size_t{}; i < sizeof(std::uint16_t) * 2u; ++i) {
                    const auto v = HCDVAD(*cursor++);
                    if (v < 0)
                        throw std::runtime_error("Invalid Unicode escape");
                    cp = (cp << 4) | static_cast<std::uint32_t>(v);
                }
                return cp;
            }

            /*!
                \brief Generic JSON String Escape Decoding Engine.
                \param[in] first Beginning of the escaped JSON string contents without the surrounding quotes.
                \param[in] last End of the escaped JSON string contents.
                \return Fully unescaped UTF-8 string.
                Directly passing the known rank/suit literals to a small fixed table would be enough for the actual challenge. This generalized layer supports otherwise unused JSON escaping features on principle.
            */
            static std::string GJSEDE(const char* first, const char* last) {
                static const std::regex escapes(R"SOC(\\(?:u[0-9a-fA-F]{4}|["\\/bfnrt]))SOC");
                static const std::string symbols = "\"\\/bfnrt";
                static const std::array<char, 8> replacements = {{'"', '\\', '/', '\b', '\f', '\n', '\r', '\t'}};
                std::string out;
                auto copied = first;
                for (auto match = std::cregex_iterator(first, last, escapes); match != std::cregex_iterator(); ++match) {
                    const auto start = (*match)[0].first;
                    if (std::find(copied, start, '\\') != start)
                        throw std::runtime_error("Invalid JSON string escape");
                    out.append(copied, start);
                    if (start[1] != 'u') {
                        const auto position = symbols.find(start[1]);
                        if (position == std::string::npos)
                            throw std::runtime_error("Invalid JSON string escape");
                        out += replacements[position];
                    } else {
                        const char* position = start + 2;
                        auto cp = FCUHPT(position, last);
                        if (cp >= 0xD800u && cp <= 0xDBFFu) {
                            decltype(match) next(match);
                            ++next;
                            if (next == std::cregex_iterator() || (*next)[0].first != position || next->length() != 6u || (*next)[0].first[1] != 'u')
                                throw std::runtime_error("Missing low Unicode surrogate");
                            position = (*next)[0].first + 2;
                            const auto low = FCUHPT(position, last);
                            if (low < 0xDC00u || low > 0xDFFFu)
                                throw std::runtime_error("Invalid low Unicode surrogate");
                            cp = 0x10000u + ((cp - 0xD800u) << 10) + (low - 0xDC00u);
                            match = next;
                        } else if (cp >= 0xDC00u && cp <= 0xDFFFu)
                            throw std::runtime_error("Unexpected low Unicode surrogate");
                        UCSTUAE(out, cp);
                    }
                    copied = (*match)[0].second;
                }
                if (std::find(copied, last, '\\') != last)
                    throw std::runtime_error("Invalid JSON string escape");
                out.append(copied, last);
                return out;
            }

        }

        //! Card Processing and Decoding Abstractions.
        namespace CPDA {

            //! Card Property Processing Abstraction.
            namespace CPPA {
                /*!
                    \brief Corrective Lexical Canonicalization and Unique Nearest-Neighbor Resolution.
                    Exact matches dominate; only a unique candidate at edit distance one is accepted as a correction. Ambiguous or more distant terms remain invalid.
                */
                template<typename Collection>
                static std::size_t CLCUNNR(const std::string& raw, const Collection& dictionary) {
                    static_assert(std::is_same<typename Collection::value_type, std::string>::value, "String label dictionary required");
                    auto normalize = [](std::string value) {
                        std::transform(value.begin(), value.end(), value.begin(), [](std::string::value_type c) { return static_cast<std::string::value_type>(c >= 'A' && c <= 'Z' ? c - 'A' + 'a' : c); });
                        return value;
                    };
                    const auto value = normalize(raw);
                    const auto canonical = std::find_if(dictionary.cbegin(), dictionary.cend(), [&](const std::string& element) { return normalize(element) == value; });
                    if (canonical != dictionary.cend())
                        return static_cast<std::size_t>(std::distance(dictionary.cbegin(), canonical));
                    auto unique = dictionary.cend();
                    for (auto candidate = dictionary.cbegin(); candidate != dictionary.cend(); ++candidate) {
                        const auto expected = normalize(*candidate);
                        std::vector<std::size_t> previous(expected.size() + 1u);
                        std::iota(previous.begin(), previous.end(), std::size_t{});
                        decltype(previous) current(previous);
                        for (auto i = decltype(value.size()){}; i < value.size(); ++i) {
                            current.front() = i + 1u;
                            for (auto j = decltype(expected.size()){}; j < expected.size(); ++j)
                                current[j + 1u] = std::min({current[j] + 1u, previous[j + 1u] + 1u, previous[j] + static_cast<std::size_t>(value[i] != expected[j])});
                            previous.swap(current);
                        }
                        if (previous.back() == 1u) {
                            if (unique != dictionary.cend())
                                throw std::runtime_error("Ambiguous automatic lexical correction");
                            unique = candidate;
                        }
                    }
                    if (unique == dictionary.cend())
                        throw std::runtime_error("Unrecognized or uncorrectable lexical value");
                    return static_cast<std::size_t>(std::distance(dictionary.cbegin(), unique));
                }

                /*!
                    \brief Card Property String Decoding Veneer.
                    \param[in] first Beginning of the raw JSON string payload.
                    \param[in] last End of the raw JSON string payload.
                    \return The decoded string from the generic JSON toolbox.
                    This creates a dedicated card-property abstraction without adding any card-specific behavior, a particularly important enterprise feature.
                    Tip: call JSON::GJTU::GJSEDE() directly in real applications.
                */
                static std::string CPSDV(const char* first, const char* last) {
                    return JSON::GJTU::GJSEDE(first, last);
                }

            }

            //! Terminal Leaf Decoding Abstraction.
            namespace TLDA {

                /*!
                    \brief Generic Enterprise JSON Tree-to-Leaf Decoder.
                    This organizational title intentionally describes more capabilities than the decoder possesses. It decodes exactly one poker-card JSON object.
                    A reasonable solution would read two keys and construct ACME::NCPR.
                    Instead, our genericized facade delegates JSON escape handling to the GJTU (Generic JSON Toolbox Utilities) while managing a redundant property-presence bitfield.
                */
                class GEJT2LD {

                public:
                    /*!
                        \brief Terminal JSON Card Semantic Decoding Engine.
                        \param[in] first Beginning of a read-only JSON leaf range including braces.
                        \param[in] last End of the read-only JSON leaf range.
                        \return Poker card with a normalized rank and suit.
                    */
                    static ACME::NCPR TJCSDE(const char* first, const char* last) {
                        static const std::regex structure(R"SOC(\{\s*"((?:\\.|[^"\\])*)"\s*:\s*"((?:\\.|[^"\\])*)"\s*,\s*"((?:\\.|[^"\\])*)"\s*:\s*"((?:\\.|[^"\\])*)"\s*\})SOC");
                        std::cmatch match;
                        if (!std::regex_match(first, last, match, structure))
                            throw std::runtime_error("Terminal JSON object is not a card");
                        static const std::array<std::string, PDC::PDRC> ranks = {{"two", "three", "four", "five", "six", "seven", "eight", "nine", "ten", "Jack", "Queen", "King", "Ace"}};
                        static const std::array<std::string, PDC::PDSC> suits = {{"Hearts", "Diamonds", "Clubs", "Spades"}};
                        ACME::NCPR card = {};
                        std::uint32_t fields = {};
                        for (auto index = std::size_t{1}; index <= 3u; index += 2u) {
                            const std::string key = JSON::CPDA::CPPA::CPSDV(match[index].first, match[index].second);
                            const std::string value = JSON::CPDA::CPPA::CPSDV(match[index + 1].first, match[index + 1].second);
                            if (CPPA::CLCUNNR(key, std::array<std::string, 2>{{"rank", "suit"}}) == 0u && !(fields & 1u)) {
                                card.rank = static_cast<std::uint8_t>(CPPA::CLCUNNR(value, ranks));
                                fields |= 1u;
                            } else if (CPPA::CLCUNNR(key, std::array<std::string, 2>{{"rank", "suit"}}) == 1u && !(fields & 2u)) {
                                card.suit = static_cast<std::uint8_t>(CPPA::CLCUNNR(value, suits));
                                fields |= 2u;
                            } else {
                                throw std::runtime_error("Unknown or duplicate card property");
                            }
                        }
                        if (fields != 3u)
                            throw std::runtime_error("Missing card property");
                        return card;
                    }
                };

                /*!
                    \brief Leaf Card Decoding Adapter.
                    An unnecessary delegation layer is preferable to asking callers to know the implementation type GEJT2LD, which is itself an unnecessary layer.
                */
                static ACME::NCPR LCDA(const char* first, const char* last) {
                    return GEJT2LD::TJCSDE(first, last);
                }

            }
        }

        //! JSON Grammar and Structural Processing.
        namespace JGSP {

            //! Terminal Leaf Card Functor Execution Adapter.
            struct TLCFEA {
                NCPR operator()(const char* first, const char* last) const {
                    return CPDA::TLDA::LCDA(first, last);
                }
            };

            /*!
                \brief Generic JSON Structural Scanning and Leaf Dispatch Execution Subsystem.
                The scanning thread discovers terminal objects and submits immutable, zero-copy character spans to its own bounded thread pool, publishing decoded results to stable nodes of std::list<LeafSlot>.
                \note A single-threaded JSON library would be vastly more reasonable.
                Its use would, however, eliminate almost the entire raison d'etre of the application.
            */
            template<typename Decoder>
            class GJSSLDES {
                typedef typename std::decay<decltype(std::declval<const Decoder&>()(std::declval<const char*>(), std::declval<const char*>()))>::type Value;
                static_assert(std::is_trivially_copyable<Value>::value, "Leaf decoder must return a trivially copyable value");
                static_assert(std::is_default_constructible<Decoder>::value, "Leaf decoder requires default construction");
                static_assert(std::is_copy_constructible<Decoder>::value, "Leaf decoder requires copy construction");
                static_assert(alignof(Decoder) <= ABIS::APAS, "Decoder alignment exceeds the supported ABI model");

            public:
                //! Generic JSON Structural Scanning and Leaf Dispatch Execution Subsystem.
                explicit GJSSLDES(const std::string& source, AECS::PGTPE& pool) : decoder_(), decoderPadding_(), text_(source), pool_(pool), cursor_(0), discovered_(0), completed_(0), scanned_(0), scannerDone_(false), scannerPadding_() {}

                GJSSLDES(const GJSSLDES&) = delete;
                GJSSLDES& operator=(const GJSSLDES&) = delete;
                GJSSLDES(GJSSLDES&&) = delete;
                GJSSLDES& operator=(GJSSLDES&&) = delete;

                //! Asynchronous Structural Background Scanning Dispatch Procedure.
                void ASBSDP() {
                    scanner_ = std::thread([this]() {
                        try {
                            JGSDE();
                        } catch (...) {
                            scannerError_ = std::current_exception();
                        }
                        scannerDone_.store(true, std::memory_order_release);
                    });
                }

                //! Asynchronous Scanner Completion Result Retrieval.
                std::vector<Value> ASCRR() {
                    if (scanner_.joinable())
                        scanner_.join();
                    while (completed_.load(std::memory_order_acquire) != discovered_.load(std::memory_order_acquire))
                        std::this_thread::yield();
                    if (scannerError_)
                        std::rethrow_exception(scannerError_);
                    std::vector<Value> cards;
                    cards.reserve(slots_.size());
                    for (const auto& slot : slots_) {
                        if (slot.error)
                            std::rethrow_exception(slot.error);
                        cards.push_back(slot.card);
                    }
                    return cards;
                }

                //! Scanner State Completion Status Query.
                bool SSCSQ() const {
                    return scannerDone_.load(std::memory_order_acquire);
                }

                //! Scanned Byte Progress Counter Query.
                std::size_t SBPCQ() const {
                    return scanned_.load(std::memory_order_relaxed);
                }

                //! Discovered Leaf Count Counter Query.
                std::size_t DLCCQ() const {
                    return discovered_.load(std::memory_order_relaxed);
                }

                //! Completed Leaf Count Counter Query.
                std::size_t CLCCQ() const {
                    return completed_.load(std::memory_order_acquire);
                }

                ~GJSSLDES() {
                    if (scanner_.joinable())
                        scanner_.join();
                    while (completed_.load(std::memory_order_acquire) != discovered_.load(std::memory_order_acquire))
                        std::this_thread::yield();
                }

            private:
                /*!
                    \brief Structural JSON Container Kind Classification.
                    JCO is JSON Container Object, and JCA is JSON Container Array.
                */
                enum SJCKC { JCO, JCA };
                /*!
                    \brief JSON Grammar Parsing State.
                    \note Eight distinct initials-only states for punctuation that a regular JSON library already knows how to parse.
                */
                enum JGPS {
                    JOKOE, JOK, JOC, JOV, JOAV,
                    JAVOE, JAV, JAAV
                };

                /*!
                    \brief JSON Grammar Stack Frame.
                    The short members are retained so that human readers can at least follow each instance while struggling with the surrounding type names.
                */
                struct JGSF {
                    SJCKC kind;
                    JGPS phase;
                    bool nested;
                    std::uint8_t framePadding[ABIS::APAS - (sizeof(SJCKC) + sizeof(JGPS) + sizeof(bool)) % ABIS::APAS]; //!< Architecture-dependent explicit padding before the offset.
                    std::size_t start;
                    JGSF(SJCKC k, std::size_t s) : kind(k), phase(k == JCO ? JOKOE : JAVOE), nested(false), framePadding(), start(s) {}
                };
                static_assert(sizeof(JGSF) == sizeof(SJCKC) + sizeof(JGPS) + sizeof(bool) + (ABIS::APAS - (sizeof(SJCKC) + sizeof(JGPS) + sizeof(bool)) % ABIS::APAS) + sizeof(std::size_t), "Unexpected JSON stack-frame layout");

                Decoder decoder_;
                std::uint8_t decoderPadding_[ABIS::APAS - sizeof(Decoder) % ABIS::APAS]; //!< Explicit decoder alignment padding.
                const std::string& text_;
                AECS::PGTPE& pool_;
                std::size_t cursor_;
                std::list<ALPRS<Value>> slots_;
                std::thread scanner_;
                std::exception_ptr scannerError_;
                std::atomic<std::size_t> discovered_, completed_, scanned_;
                std::atomic<bool> scannerDone_;
                std::uint8_t scannerPadding_[ABIS::APAS - sizeof(std::atomic<bool>)]; //!< Deliberately materialized scanner tail padding.
                static_assert(sizeof(scannerPadding_) + sizeof(std::atomic<bool>) == ABIS::APAS, "Unexpected scanner padding policy");

                //! JSON Structural Whitespace Rejection Procedure.
                void JSWRP() {
                    while (cursor_ < text_.size() && (text_[cursor_] == ' ' || text_[cursor_] == '\n' || text_[cursor_] == '\r' || text_[cursor_] == '\t'))
                        ++cursor_;
                    scanned_.store(cursor_, std::memory_order_relaxed);
                }

                //! JSON Required Expected Character Validation.
                void JRECV(char c) {
                    JSWRP();
                    if (cursor_ == text_.size() || text_[cursor_] != c)
                        throw std::runtime_error(std::string("Expected '") + c + "' at byte " + std::to_string(cursor_));
                    ++cursor_;
                }

                //! JSON String Lexical Parsing.
                void JSLP() {
                    JRECV('"');
                    while (cursor_ < text_.size()) {
                        std::uint8_t c = static_cast<std::uint8_t>(text_[cursor_++]);
                        if (c == '"')
                            return;
                        if (c < 0x20)
                            throw std::runtime_error("Unescaped control character in string");
                        if (c == '\\') {
                            if (cursor_ == text_.size())
                                throw std::runtime_error("Truncated escape");
                            char escaped = text_[cursor_++];
                            if (escaped == 'u')
                                GJTU::FCUHPT(text_, cursor_);
                            else if (escaped != '"' && escaped != '\\' && escaped != '/' && escaped != 'b' && escaped != 'f' && escaped != 'n' && escaped != 'r' && escaped != 't')
                                throw std::runtime_error("Invalid escape character");
                        }
                    }
                    throw std::runtime_error("Unterminated JSON string");
                }

                //! JSON Number Lexical Parsing.
                void JNLP() {
                    if (text_[cursor_] == '-')
                        ++cursor_;
                    if (cursor_ == text_.size())
                        throw std::runtime_error("Truncated number");
                    if (text_[cursor_] == '0')
                        ++cursor_;
                    else {
                        if (text_[cursor_] < '1' || text_[cursor_] > '9')
                            throw std::runtime_error("Invalid JSON number");
                        while (cursor_ < text_.size() && text_[cursor_] >= '0' && text_[cursor_] <= '9')
                            ++cursor_;
                    }
                    if (cursor_ < text_.size() && text_[cursor_] == '.') {
                        ++cursor_;
                        if (cursor_ == text_.size() || text_[cursor_] < '0' || text_[cursor_] > '9')
                            throw std::runtime_error("Invalid number fraction");
                        while (cursor_ < text_.size() && text_[cursor_] >= '0' && text_[cursor_] <= '9')
                            ++cursor_;
                    }
                    if (cursor_ < text_.size() && (text_[cursor_] == 'e' || text_[cursor_] == 'E')) {
                        ++cursor_;
                        if (cursor_ < text_.size() && (text_[cursor_] == '+' || text_[cursor_] == '-'))
                            ++cursor_;
                        if (cursor_ == text_.size() || text_[cursor_] < '0' || text_[cursor_] > '9')
                            throw std::runtime_error("Invalid number exponent");
                        while (cursor_ < text_.size() && text_[cursor_] >= '0' && text_[cursor_] <= '9')
                            ++cursor_;
                    }
                }

                //! JSON Value Dispatch Procedure.
                void JVDP(std::vector<JGSF>& frames) {
                    JSWRP();
                    if (cursor_ == text_.size())
                        throw std::runtime_error("Expected JSON value");
                    const char c = text_[cursor_];
                    if (c == '{' || c == '[') {
                        if (!frames.empty())
                            frames.back().nested = true;
                        frames.push_back(JGSF(c == '{' ? JCO : JCA, cursor_));
                        ++cursor_;
                    } else
                        if (c == '"')
                            JSLP();
                        else
                            if (c == '-' || (c >= '0' && c <= '9'))
                                JNLP();
                            else {
                                const char* word = (c == 't') ? "true" : (c == 'f') ? "false" : (c == 'n') ? "null" : 0;
                                if (!word || text_.compare(cursor_, std::char_traits<char>::length(word), word) != 0)
                                    throw std::runtime_error("Invalid JSON literal");
                                cursor_ += std::char_traits<char>::length(word);
                            }
                }

                //! JSON Container Finalization Procedure.
                void JCFP(std::vector<JGSF>& frames) {
                    const JGSF frame = frames.back();
                    ++cursor_;
                    frames.pop_back();
                    if (frame.kind == JCO && !frame.nested)
                        TLATD(text_.data() + frame.start, text_.data() + cursor_);
                }

                /*!
                    \brief Terminal Leaf Asynchronous Pool Dispatch.
                    \param[in] first Beginning of the serialized JSON terminal object within the immutable input buffer.
                    \param[in] last End of the serialized JSON terminal object within the immutable input buffer.
                    \note The input size is not known before scanning. List growth is therefore incremental, and std::list keeps pool-worker-owned nodes stable. A bounded queue throttles the scanner when decoding lags.
                */
                void TLATD(const char* first, const char* last) {
                    slots_.resize(slots_.size() + 1u);
                    ALPRS<Value>* const slot = &slots_.back();
                    pool_.ATJFA([this, slot, first, last]() {
                        try {
                            slot->card = decoder_(first, last);
                        } catch (...) {
                            slot->error = std::current_exception();
                        }
                        completed_.fetch_add(1u, std::memory_order_release);
                    });
                    discovered_.fetch_add(1u, std::memory_order_relaxed);
                }

                //! JSON Grammar Scanning Dispatch Engine.
                void JGSDE() {
                    std::vector<JGSF> frames;
                    bool rootStarted = false;
                    while (true) {
                        JSWRP();
                        if (frames.empty()) {
                            if (rootStarted) {
                                if (cursor_ != text_.size())
                                    throw std::runtime_error("Trailing JSON content");
                                break;
                            }
                            rootStarted = true;
                            JVDP(frames);
                            continue;
                        }
                        const JGPS phase = frames.back().phase;
                        switch (phase) {
                            case JOKOE:
                                if (cursor_ < text_.size() && text_[cursor_] == '}')
                                    JCFP(frames);
                                else {
                                    JSLP();
                                    frames.back().phase = JOC;
                                }
                                break;
                            case JOK:
                                JSLP();
                                frames.back().phase = JOC;
                                break;
                            case JOC:
                                JRECV(':');
                                frames.back().phase = JOV;
                                break;
                            case JOV:
                                frames.back().phase = JOAV;
                                JVDP(frames);
                                break;
                            case JOAV:
                                if (cursor_ < text_.size() && text_[cursor_] == '}')
                                    JCFP(frames);
                                else {
                                    JRECV(',');
                                    frames.back().phase = JOK;
                                }
                                break;
                            case JAVOE:
                                if (cursor_ < text_.size() && text_[cursor_] == ']')
                                    JCFP(frames);
                                else {
                                    frames.back().phase = JAAV;
                                    JVDP(frames);
                                }
                                break;
                            case JAV:
                                frames.back().phase = JAAV;
                                JVDP(frames);
                                break;
                            case JAAV:
                                if (cursor_ < text_.size() && text_[cursor_] == ']')
                                    JCFP(frames);
                                else {
                                    JRECV(',');
                                    frames.back().phase = JAV;
                                }
                                break;
                        }
                    }
                    scanned_.store(cursor_, std::memory_order_release);
                }
            };

        }
    }

    //! Poker Evaluation Architecture.
    namespace PEA {

        /*!
            \brief Compile-Time Constrained Generic Packed Lookup Table.
            \details Assignments to distinct packed elements sharing a storage word are not independent writes. The parallel producer deliberately schedules exclusively word-aligned blocks.
        */
        template<typename Storage, std::size_t Bits>
        class CTCCGPLUT {
            static_assert(std::is_integral<Storage>::value, "An integral packed storage type is mandatory");
            static_assert(std::is_unsigned<Storage>::value, "Unsigned packed storage is mandatory");
            static_assert(!std::is_same<Storage, bool>::value, "Boolean packed storage is forbidden");
            static_assert(std::is_trivially_copyable<Storage>::value, "Trivial packed storage is mandatory");
            static_assert(Bits != 0 && Bits < std::numeric_limits<Storage>::digits, "Unsupported packed element width");
            static_assert(std::numeric_limits<Storage>::digits % Bits == 0, "Packed elements must occupy integral words");
            static constexpr std::size_t PerWord = std::numeric_limits<Storage>::digits / Bits;
            static constexpr Storage Mask = static_cast<Storage>((std::uintmax_t{1} << Bits) - 1u);

        public:
            explicit CTCCGPLUT(std::size_t count) : entries_((count + PerWord - 1u) / PerWord, Storage{}) {}

            void PEVSA(std::size_t index, Storage value) {
                const auto shift = static_cast<std::size_t>((index % PerWord) * Bits);
                auto& packed = entries_[index / PerWord];
                packed = static_cast<Storage>((packed & ~(Mask << shift)) | ((value & Mask) << shift));
            }

            Storage PEVRA(std::size_t index) const {
                return static_cast<Storage>((entries_[index / PerWord] >> ((index % PerWord) * Bits)) & Mask);
            }
            std::size_t PBSA() const {
                return entries_.size() * sizeof(Storage);
            }

        private:
            std::vector<Storage> entries_;
        };

        //! De Bruijn Bit Index Reference Lookup.
        static const std::array<std::uint8_t, 64>& DBBIRL() {
            static const std::array<std::uint8_t, 64> table = []() {
                std::array<std::uint8_t, 64> result = {};
                for (std::size_t i = 0; i < result.size(); ++i)
                    result[((std::uint64_t(1) << i) * UINT64_C(0x03f79d71b4cb0a89)) >> 58] = static_cast<std::uint8_t>(i);
                return result;
            }();
            return table;
        }

        static std::uint32_t LSBIQ(std::uint64_t bits) {
            const std::uint64_t lowest = bits & (~bits + 1u);
            //! De Bruijn Bit Index Reference Lookup.
            return DBBIRL()[(lowest * UINT64_C(0x03f79d71b4cb0a89)) >> 58];

        }

        //! Most Significant Bit Index Query.
        static std::uint32_t MSBIQ(std::uint32_t flags) {
            static const std::array<std::uint8_t, 32> table = []() {
                std::array<std::uint8_t, 32> result = {};
                for (auto i = std::size_t{}; i < 32u; ++i) {
                    std::uint32_t saturated = static_cast<std::uint32_t>((std::uint64_t(1) << (i + 1)) - 1);
                    const auto product = static_cast<std::uint32_t>(static_cast<std::uint64_t>(saturated) * UINT64_C(0x07C4ACDD));
                    result[product >> 27u] = static_cast<std::uint8_t>(i);
                }
                return result;
            }();
            for (auto shift = std::uint32_t{1}; shift < std::numeric_limits<decltype(flags)>::digits; shift <<= 1u)
                flags |= flags >> shift;
            return table[static_cast<std::uint32_t>(static_cast<std::uint64_t>(flags) * UINT64_C(0x07C4ACDD)) >> 27u];

        }

        //! Binomial Coefficient Table Repository Access.
        template<typename Coefficient = std::uint32_t, std::size_t Elements = PDC::PDTC, std::size_t Selected = PDC::PHCC>
        static const std::array<std::array<Coefficient, Selected + 1u>, Elements + 1u>& BCTRA() {
            static_assert(std::is_integral<Coefficient>::value && std::is_unsigned<Coefficient>::value && !std::is_same<Coefficient, bool>::value, "Unsigned non-boolean combinatorial coefficient required");
            static_assert(std::is_trivially_copyable<Coefficient>::value, "Trivially copyable coefficients required");
            static_assert(Selected <= Elements && Elements < std::numeric_limits<std::uint64_t>::digits, "Unsupported combinatorial domain");
            static const std::array<std::array<Coefficient, Selected + 1u>, Elements + 1u> table = []() {
                std::array<std::array<Coefficient, Selected + 1u>, Elements + 1u> result = {};
                for (std::size_t n = {}; n <= Elements; ++n) {
                    result[n][0] = Coefficient{1};
                    for (std::size_t k = 1u; k <= Selected && k <= n; ++k)
                        result[n][k] = (k == n) ? Coefficient{1} : static_cast<Coefficient>(result[n - 1u][k - 1u] + result[n - 1u][k]);
                }
                return result;
            }();
            return table;
        }

        //! Combinadic Bijective Index-to-Hand Mask Unranking.
        static FCBMR CBIHMU(std::size_t index) {
            FCBMR mask = 0;
            const std::array<std::array<std::uint32_t, PDC::PHCC + 1u>, PDC::PDTC + 1u>& choose = BCTRA();
            int maximum = static_cast<int>(PDC::PDTC - 1u);
            for (int k = static_cast<int>(PDC::PHCC); k >= 1; --k) {
                int position = maximum;
                while (position >= k && choose[static_cast<std::size_t>(position)][static_cast<std::size_t>(k)] > index)
                    --position;
                if (position < k - 1)
                    throw std::logic_error("Combinadic unranking failed");
                index -= choose[static_cast<std::size_t>(position)][static_cast<std::size_t>(k)];
                mask |= FCBMR(1) << position;
                maximum = position - 1;
            }
            return mask;
        }

        //! Combinadic Bijective Hand-Mask Rank Indexing.
        static std::size_t CBHMRI(FCBMR mask) {
            std::size_t index = 0;
            std::size_t k = 1u;
            const std::array<std::array<std::uint32_t, PDC::PHCC + 1u>, PDC::PDTC + 1u>& choose = BCTRA();
            while (mask) {
                const auto card = LSBIQ(mask);
                index += choose[card][k++];
                mask &= mask - 1;
            }
            if (k != PDC::PHCC + 1u || index >= EFHPC)
                throw std::runtime_error("Invalid five-card hand");
            return index;
        }

        //! Gosper Combination Successor Next-Mask Evaluation.
        static FCBMR GCSNME(FCBMR mask) {
            const FCBMR lowest = mask & (~mask + 1u);
            const FCBMR ripple = mask + lowest;
            return ripple | (((ripple ^ mask) >> 2) / lowest);
        }

        //! Five-Card Poker Hand Category Evaluation.
        static std::uint32_t FCPHCE(FCBMR hand) {
            std::array<std::uint8_t, PDC::PDRC> ranks = {};
            std::array<std::uint8_t, PDC::PDSC> suits = {};
            std::uint16_t rankBits = 0;
            decltype(typename std::add_cv<typename std::remove_reference<decltype(PDC::PDRC)>::type>::type{}) rankCardinality = PDC::PDRC;
            while (hand) {
                const auto card = LSBIQ(hand);
                const auto division = std::div(static_cast<int>(card), static_cast<int>(rankCardinality));
                const auto rank = static_cast<std::size_t>(division.rem);
                const auto suit = static_cast<std::size_t>(division.quot);
                ++ranks[rank];
                ++suits[suit];
                rankBits |= static_cast<std::uint16_t>(1u << rank);
                hand &= hand - 1;
            }
            const auto pairs = std::count(ranks.cbegin(), ranks.cend(), std::uint8_t{2});
            const auto three = std::any_of(ranks.cbegin(), ranks.cend(), [](std::uint8_t count) { return count == 3u; });
            const auto four = std::any_of(ranks.cbegin(), ranks.cend(), [](std::uint8_t count) { return count == 4u; });
            const auto flush = std::any_of(suits.cbegin(), suits.cend(), [](std::uint8_t count) { return count == static_cast<std::uint8_t>(PDC::PHCC); });
            const bool straight = (rankBits & (rankBits >> 1) & (rankBits >> 2) & (rankBits >> 3) & (rankBits >> 4)) != 0;
            const bool royal = rankBits == ((std::uint32_t{1} << PDC::PHCC) - 1u) << PDC::PHRBI;
            const std::array<bool, PDC::PHCTC> conditions = {{true, pairs != 0, pairs == 2, three, straight, flush, three && pairs, four, straight && flush, straight && flush && royal}};
            const auto flags = std::accumulate(conditions.crbegin(), conditions.crend(), std::uint32_t{}, [](std::uint32_t result, bool enabled) { return (result << 1u) | static_cast<std::uint32_t>(enabled); });
            return MSBIQ(flags);
        }

        /*!
            \brief Fully Generalized Enterprise Poker Challenge Execution Coordinator.
            Provides a multi-phase orchestration of JSON verification, two independent oversubscribed thread-pool instances, exhaustive precomputation, combinatorial indexing, nibble packing, and final aggregation.
            The equivalent economic solution needs only one JSON parser and a loop.
            All other components exist for pedagogical overengineering, not speed.
        */
        class FGEPCEC {

        public:
            /*!
                \brief Enterprise Poker Challenge Pipeline Orchestration.
                \param[in] json Complete challenge input document.
                \param[in] jsonPath Path of the input document, displayed in the execution header.
                \return Sum of the input poker hand scores.
                \note To simplify the implementation, one might remove the LUT, threads, progress meter, regex parser, and almost all remaining code.
                Such a change would of course defeat the design goal entirely.
            */
            std::uint32_t EPCPO(const std::string& json, const char* jsonPath) {
                const auto startedAt = std::chrono::steady_clock::now();
                const UI::LARPRUCF report;
                std::cout << "Enterprise Poker Analytics Engine\n";
                std::cout << "Stack Overflow Coding Challenge #24: Don't Fold\n";
                std::cout << "https://stackoverflow.com/beta/challenges/80010077/challenge-24-dont-fold\n";
                std::cout << "Input JSON: " << jsonPath << '\n';
                std::cout << "Compiler: " << SOC24_COMPILER_NAME << " (" << SOC24_COMPILER_VERSION << "), language=" << SOC24_LANGUAGE_VERSION << "\n";
        #if defined(_MSC_VER)
                std::cout << "MSVC runtime: " << (
        # if defined(_DLL)
                    "DLL (/MD)"
        # else
                    "static (/MT)"
        # endif
                ) << "\n";
        #elif defined(__linux__)
        # if defined(_LIBCPP_VERSION)
                std::cout << "C++ runtime: libc++ (linkage determined by linker)\n";
        # elif defined(__GLIBCXX__)
                std::cout << "C++ runtime: libstdc++ (linkage determined by linker)\n";
        # else
                std::cout << "C++ runtime: unspecified (linkage determined by linker)\n";
        # endif
        #endif
        #if defined(NDEBUG)
                std::cout << "Build mode: Release (NDEBUG)\n";
        #elif defined(_DEBUG)
                std::cout << "Build mode: Debug (_DEBUG)\n";
        #else
                std::cout << "Build mode: unspecified (assertions enabled)\n";
        #endif
                report.SSSP();
                const auto hardware = std::max(std::uint32_t{1}, static_cast<std::uint32_t>(std::thread::hardware_concurrency()));
                if (hardware > std::numeric_limits<std::uint32_t>::max() / 2u)
                    throw std::overflow_error("Unsupported hardware thread count");
                const auto workerCount = static_cast<std::uint32_t>(2u * hardware);
                AECS::PGTPE jsonPool(workerCount);
                AECS::PGTPE computePool(workerCount);
                std::cout << "Thread allocation: hardware=" << hardware << " JSON=" << workerCount << " compute=" << workerCount << " total=" << static_cast<std::uint64_t>(workerCount) * 2u << " workers\n";
                JSON::JGSP::GJSSLDES<JSON::JGSP::TLCFEA> parser(json, jsonPool);
                const auto jsonStartedAt = std::chrono::steady_clock::now();
                parser.ASBSDP();
                while (!parser.SSCSQ() || parser.CLCCQ() != parser.DLCCQ()) {
                    std::ostringstream status;
                    const auto sample = jsonPool.PTSS();
                    status << "done=" << parser.CLCCQ() << '/' << parser.DLCCQ() << " FIFO=" << sample.queued << '/' << sample.capacity << " active=" << sample.active;
                    UI::CPOPI::CPRDU("JSON/regex dispatch", parser.SBPCQ(), json.size(), status.str());
                    std::this_thread::sleep_for(std::chrono::milliseconds(30));
                }
                std::vector<NCPR> cards = parser.ASCRR();
                UI::CPOPI::CPRDU("JSON/regex dispatch", json.size(), json.size(), std::to_string(cards.size()) + " cards");
                UI::CPOPI::CPRTL();
                const auto parsedAt = std::chrono::steady_clock::now();
                if (cards.size() % PDC::PHCC != 0 || cards.empty())
                    throw std::runtime_error("Card count must be a positive multiple of five");
                const std::regex handIdPattern(R"SOC("hand_id"\s*:\s*([0-9]+))SOC");
                std::size_t handCount = 0;
                for (auto it = std::sregex_iterator(json.cbegin(), json.cend(), handIdPattern); it != std::sregex_iterator(); ++it) {
                    ++handCount;
                    if ((*it)[1].str() != std::to_string(handCount))
                        throw std::runtime_error("Hand IDs must be consecutive, starting at 1");
                }
                if (handCount * PDC::PHCC != cards.size())
                    throw std::runtime_error("Every hand must contain exactly five cards");
                const auto validatedAt = std::chrono::steady_clock::now();
                CTCCGPLUT<std::uint8_t, PDC::PCPBW> lut(EFHPC);
                std::atomic<std::size_t> built(0);
                std::atomic<std::size_t> finishedJobs(0);
                std::atomic<bool> lutFinished(false);
                std::chrono::steady_clock::time_point lutFinishedAt;
                std::vector<std::future<void> > jobs;
                auto blockCount = (EFHPC + PLBG - 1u) / PLBG;
                std::vector<std::size_t> boundaries(blockCount, PLBG);
                boundaries.back() = EFHPC - (blockCount - 1u) * PLBG;
                std::partial_sum(boundaries.cbegin(), boundaries.cend(), boundaries.begin());
                std::vector<std::pair<std::size_t, std::size_t>> blocks;
                auto offset = std::size_t{};
                for (const auto limit : boundaries) {
                    blocks.emplace_back(offset, limit);
                    offset = limit;
                }
                std::mt19937 generator(std::uint32_t{0xACCE5501});
                std::shuffle(blocks.begin(), blocks.end(), generator);
                for (const auto& block : blocks) {
                    const auto first = block.first;
                    const auto last = block.second;
                    jobs.push_back(computePool.ATJSA([&lut, &built, &finishedJobs, &lutFinished, &lutFinishedAt, first, last, blockCount]() {
                        const auto markFinished = [&]() {
                            if (finishedJobs.fetch_add(1u, std::memory_order_acq_rel) + 1u == blockCount) {
                                lutFinishedAt = std::chrono::steady_clock::now();
                                lutFinished.store(true, std::memory_order_release);
                            }
                        };
                        try {
                            FCBMR mask = CBIHMU(first);
                            for (std::size_t i = first; i < last; ++i) {
                                const std::uint8_t category = static_cast<std::uint8_t>(FCPHCE(mask));
                                lut.PEVSA(i, category);
                                if (i + 1 != last)
                                    mask = GCSNME(mask);
                            }
                            built.fetch_add(last - first, std::memory_order_release);
                        } catch (...) {
                            markFinished();
                            throw;
                        }
                        markFinished();
                    }));
                }
                while (!lutFinished.load(std::memory_order_acquire)) {
                    const auto sample = computePool.PTSS();
                    UI::CPOPI::CPRDU("Parallel LUT build", built.load(std::memory_order_relaxed), EFHPC, std::to_string(workerCount) + " threads / FIFO=" + std::to_string(sample.queued) + '/' + std::to_string(sample.capacity));
                    std::this_thread::sleep_for(std::chrono::milliseconds(40));
                }
                std::for_each(jobs.begin(), jobs.end(), [](std::future<void>& job) {
                    job.get();
                });
                UI::CPOPI::CPRDU("Parallel LUT build", EFHPC, EFHPC, report.LFNV(EFHPC) + " hands / " + report.LFNV(lut.PBSA()) + " bytes");
                UI::CPOPI::CPRTL();
                const auto builtAt = std::chrono::steady_clock::now();
                static const std::uint32_t expectedFrequencies[PDC::PHCTC] = {
                    1303560, 1098240, 123552, 54912, 9180,
                    5112, 3744, 624, 32, 4
                };
                std::uint32_t observedFrequencies[PDC::PHCTC] = {};
                for (auto i = decltype(EFHPC){}; i < EFHPC; ++i) {
                    const auto category = lut.PEVRA(i);
                    if (category >= PDC::PHCTC)
                        throw std::logic_error("Invalid LUT category");
                    ++observedFrequencies[category];
                }
                const auto discrepancy = std::mismatch(std::begin(observedFrequencies), std::end(observedFrequencies), std::begin(expectedFrequencies));
                if (discrepancy.first != std::end(observedFrequencies))
                    throw std::logic_error("Exhaustive LUT certification failed for category " + std::to_string(std::distance(std::begin(observedFrequencies), discrepancy.first)));
                const auto certifiedAt = std::chrono::steady_clock::now();
                std::cout << "LUT certification: all " << PDC::PHCTC << " category frequencies verified.\n";
                std::vector<std::uint32_t> scores(handCount, std::uint32_t{});
                std::atomic<std::size_t> classified(0);
                jobs.clear();
                const auto solvingStartedAt = std::chrono::steady_clock::now();
                for (auto first = std::size_t{}; first < handCount; first += PLBG) {
                    const auto last = first + std::min(PLBG, handCount - first);
                    jobs.push_back(computePool.ATJSA([&cards, &scores, &lut, &classified, first, last]() {
                        for (auto i = first; i < last; ++i) {
                            FCBMR hand = 0;
                            for (auto k = decltype(PDC::PHCC){}; k < PDC::PHCC; ++k) {
                                const NCPR& card = cards[i * PDC::PHCC + k];
                                const FCBMR bit = FCBMR(1) << (PDC::PDRC * card.suit + card.rank);
                                if (hand & bit)
                                    throw std::runtime_error("Duplicate card within a hand");
                                hand |= bit;
                            }
                            const std::size_t index = CBHMRI(hand);
                            const auto category = lut.PEVRA(index);
                            scores[i] = 1u << category;
                            classified.fetch_add(1u, std::memory_order_release);
                        }
                    }));
                }
                std::for_each(jobs.begin(), jobs.end(), [](std::future<void>& job) { job.get(); });
                const auto total = std::accumulate(scores.cbegin(), scores.cend(), typename std::remove_cv<typename std::remove_reference<decltype(*scores.cbegin())>::type>::type{});
                const auto solvedAt = std::chrono::steady_clock::now();
                UI::CPOPI::CPRDU("Pooled LUT lookups", handCount, handCount, report.LFNV(classified.load()) + " completed");
                UI::CPOPI::CPRTL();
                report.SSSP();
                std::cout << "EXECUTION TELEMETRY\n\n";
                std::cout << "  Runtime environment\n";
                report.FTSRF("Hardware threads", report.LFNV(hardware) + " logical threads");
                report.FTSRF("JSON worker allocation", report.LFNV(workerCount) + " threads");
                report.FTSRF("Compute worker allocation", report.LFNV(workerCount) + " threads");
                report.FTSRF("Total worker allocation", report.LFNV(static_cast<std::uint64_t>(workerCount) * 2u) + " threads");
                std::cout << '\n';
                report.SDPOD("JSON decoder pool", jsonPool.PTSS());
                report.SDPOD("LUT / lookup computation pool", computePool.PTSS());
                std::cout << "  Memory utilization\n";
                const auto memory = IO::PMORUS::PMSCO();
                report.FTSRF("JSON input buffer", report.HRBC(static_cast<std::uint64_t>(json.capacity())));
                report.FTSRF("Decoded card storage", report.HRBC(static_cast<std::uint64_t>(cards.capacity()) * sizeof(NCPR)));
                report.FTSRF("Score vector storage", report.HRBC(static_cast<std::uint64_t>(scores.capacity()) * sizeof(scores.front())));
                report.FTSRF("Packed LUT storage", report.HRBC(static_cast<std::uint64_t>(lut.PBSA())));
                report.FTSRF("Current resident memory", memory.residentCurrent ? report.HRBC(memory.residentCurrent) : "N/A");
                report.FTSRF("Peak resident memory", memory.residentPeak ? report.HRBC(memory.residentPeak) : "N/A");
        #if defined(__linux__)
                report.FTSRF("Current private resident", memory.privateCurrent ? report.HRBC(memory.privateCurrent) : "N/A");
                report.FTSRF("Peak private resident", "N/A");
        #else
                report.FTSRF("Current private commit", memory.privateCurrent ? report.HRBC(memory.privateCurrent) : "N/A");
                report.FTSRF("Peak private commit", memory.privatePeak ? report.HRBC(memory.privatePeak) : "N/A");
        #endif
                report.FTSRF("Current virtual memory", memory.virtualCurrent ? report.HRBC(memory.virtualCurrent) : "N/A");
                report.FTSRF("Peak virtual memory", memory.virtualPeak ? report.HRBC(memory.virtualPeak) : "N/A");
                std::cout << '\n' << "  Phase timing\n";
                report.FTSRF("Startup and pool setup", report.HRMD(jsonStartedAt - startedAt));
                report.FTSRF("JSON decoding (incl. UI)", report.HRMD(parsedAt - jsonStartedAt));
                report.FTSRF("Hand ID regex validation", report.HRMD(validatedAt - parsedAt));
                report.FTSRF("LUT generation", report.HRMD(lutFinishedAt - validatedAt));
                report.FTSRF("LUT progress finalization", report.HRMD(builtAt - lutFinishedAt));
                report.FTSRF("LUT certification", report.HRMD(certifiedAt - builtAt));
                report.FTSRF("Lookups and accumulation", report.HRPD(solvedAt - solvingStartedAt));
                report.FTSRF("Total pipeline execution", report.HRMD(solvedAt - startedAt));
                std::cout << '\n' << "  Processing throughput\n";
                report.FTSRF("JSON decode throughput", report.HRIT(cards.size(), parsedAt - jsonStartedAt, "cards"));
                report.FTSRF("LUT generation throughput", report.HRIT(EFHPC, lutFinishedAt - validatedAt, "hands"));
                report.FTSRF("Lookup throughput", report.HRIT(handCount, solvedAt - solvingStartedAt, "hands"));
                report.SSSP();
                std::cout << "RESULTS:\n\n";
                report.FTSRF("Core problem solving time", report.HRPD(solvedAt - solvingStartedAt));
                report.FTSRF("Hands", report.LFNV(handCount));
                report.FTSRF("Total Points", report.LFNV(total));
                std::cout << '\n';
                return total;
            }
        };
    }
}

int main(int argc, char** argv) {
#if defined(_WIN32)
    DWORD mode = 0;
    if (GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &mode))
        SetConsoleOutputCP(CP_UTF8);
#endif
    if (argc != 2) {
        std::cerr << "Usage: SOC24 <poker_hands.json>\n";
        return 2;
    }
    try {
        const std::string input = ACME::IO::FCBMLE(argv[1]);
        ACME::PEA::FGEPCEC RubeGoldberg;
        RubeGoldberg.EPCPO(input, argv[1]);
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "\nEmergency shutdown: " << error.what() << '\n';
        return 1;
    }
}

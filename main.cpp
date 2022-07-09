#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <unordered_map>
#include <assert.h>

class IdGen {
public:
    IdGen() {
        for (std::size_t i = 0; i < std::size(kAlphabet); ++i) {
            mLetterToIndex[kAlphabet[i]] = i;
        }
        reset();
    }

    virtual void setCurrentId(const std::string& id) {
        mSize = 0;
        for (int i = id.size() - 2; i >= 0; i -= 3) {
           CounterType counter = stringToCounter(id.substr(i, 2));
           mCounters[mSize++] = counter;
        }
    }

    virtual std::string operator ++ () {
        inc();
        return countersToString();
    }

private:
    using CounterType = std::int16_t; // -1...170

    void reset() {
        mSize = 1;
        mCounters[0] = 0;
        for (std::size_t i = 1; i < kMaxGroups; ++i) {
            mCounters[i] = -1; // not inited status
        }
    }

    CounterType stringToCounter(const std::string& id) const {
        assert(id.size() == 2);
        return id[1] - '1' + mLetterToIndex.at(id[0]) * 9;
    }

    static std::string counterToString(CounterType counter) {
        // 0->A1
        // 1->A2
        // ...
        assert(counter < kMaxNumInAGroup);
        char first = kAlphabet[counter / 9];
        char last = (counter % 9) + '1';
        return std::string{first, last};
    }

    std::string countersToString() const {
        std::string result;
        for (auto i = 0; i < mSize; ++i) {
            if (i > 0) {
                result.insert(0, "-");
            }
            result.insert(0, counterToString(mCounters[i]));
        }
        return result;
    }

    void inc() {
        std::size_t i = 0;
        while (i < kMaxGroups) {
            if (++mCounters[i] < kMaxNumInAGroup) {
                return;
            }
            mCounters[i] = 0;
            ++i;
            mSize = i + 1;
        }
        reset();
    }

    static constexpr char kAlphabet[] = "ABCEHIKLNOPRSTUWXYZ";
    static constexpr std::size_t kMaxGroups = 10;
    static constexpr std::size_t kMaxNumInAGroup = 9 * (26 - 7); // 1-9, A-Z except D,F,G,J,M,Q,V
    CounterType mCounters[kMaxGroups];
    std::size_t mSize;
    std::unordered_map<char, std::size_t> mLetterToIndex;
};

class IdGenMt : public IdGen {
public:
    void setCurrentId(const std::string &id) override {
        std::unique_lock<std::mutex> lock(mMutex);
        IdGen::setCurrentId(id);
    }

    std::string operator++() override {
        std::unique_lock<std::mutex> lock(mMutex);
        return IdGen::operator++();
    }

private:
    std::mutex mMutex;
};

// example
static void inc(IdGen& gen) {
    for (int i = 0; i < 1000; ++i) {
        std::cout << " " << ++gen << std::endl;
    }
}

int main(int argc, char** argv) {
    IdGenMt idGen{};

    std::thread t1(inc, std::ref(idGen));
    std::thread t2(inc, std::ref(idGen));
    std::thread t3(inc, std::ref(idGen));
    std::thread t4(inc, std::ref(idGen));
    std::thread t5(inc, std::ref(idGen));
    std::thread t6(inc, std::ref(idGen));

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();

    idGen.setCurrentId("Z9-Z9-Z9-Z9-Z9-Z9-Z9-Z9-Z9-Z9");
    std::cout << " " << ++idGen << std::endl;

    return 0;
}

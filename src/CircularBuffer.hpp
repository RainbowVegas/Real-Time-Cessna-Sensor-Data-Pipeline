#ifndef CIRCULARBUFFER_HPP
#define CIRCULARBUFFER_HPP

#include <array>
#include <optional>
#include <cstddef>

template <typename T, size_t BUFFER_SIZE>
class CircularBuffer{
    private:
        std::array<T, BUFFER_SIZE> buffer{};
        size_t head = 0;
        size_t tail = 0;
        bool full = false;

    public:
        bool isFull() const{
            return full;
        }

        bool isEmpty() const {
            return (!isFull() && (head == tail));
        }

        // Function gets size of buffer.
        size_t size() const {
            if (isFull()) return BUFFER_SIZE;
            if (head >= tail) return head - tail;
            return BUFFER_SIZE + head - tail;
        } 

        // Function writes new data to the circular buffer.
        // Checks if the buffer is full, if it is move tail over one.
        // Buffer is full when the head and tail are at the same index.
        void write(const T& data){
            buffer[head] = data;

            if (full){
                //move tail
                tail = (tail + 1) % BUFFER_SIZE;
            }

            head = (head + 1) % BUFFER_SIZE;

            full = (head == tail);
        }

        // Function return oldest data in the buffer.
        // Returns -1 if buffer is empty.
        std::optional<T> read() {
            if (isEmpty()) return std::nullopt;
            T data = buffer[tail];
            tail = (tail + 1) % BUFFER_SIZE;
            full = false;
            return data;
        }

        // Peak the tail
        std::optional<T> peakTail() const {
            if (isEmpty()) return std::nullopt;
            return buffer[tail];
        }
};

#endif // CIRCULARBUFFER_HPP
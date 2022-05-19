#include <iostream>

namespace custom_classes
{
    template <typename data_t>
    struct default_delete
    {
        void operator()(data_t *ptr) noexcept { delete ptr; }
    };

    template <typename data_t>
    struct default_delete<data_t[]>
    {
        void operator()(data_t *ptr) noexcept { delete[] ptr; }
    };

    template <typename data_t, typename Deleter = default_delete<data_t>>
    struct unique_ptr
    {
    private:
        data_t *ptr_;
        Deleter del_;

    public:
        unique_ptr(data_t *ptr = nullptr) noexcept : ptr_(ptr) {}
        unique_ptr(const unique_ptr &other) = delete;
        unique_ptr &operator=(const unique_ptr &other) = delete;
        unique_ptr(unique_ptr &&other) noexcept : ptr_(other.ptr_)
        {
            other.ptr_ = nullptr;
        }
        unique_ptr &operator=(unique_ptr &&other) noexcept
        {
            std::swap(ptr_, other.ptr_);
            return *this;
        }
        ~unique_ptr() { del_(ptr_); }
        data_t *operator->() { return ptr_; }
        data_t &operator*() { return *(ptr_); }
    };

    template <typename data_t, typename Deleter>
    struct unique_ptr<data_t[], Deleter>
    {
    private:
        data_t *ptr_;
        Deleter del_;

    public:
        unique_ptr(data_t *ptr = nullptr) noexcept : ptr_(ptr) {}
        unique_ptr(const unique_ptr &other) = delete;
        unique_ptr &operator=(const unique_ptr &other) = delete;
        unique_ptr(unique_ptr &&other) noexcept : ptr_(other.ptr_)
        {
            other.ptr_ = nullptr;
        }
        unique_ptr &operator=(unique_ptr &&other) noexcept
        {
            std::swap(ptr_, other.ptr_);
            return *this;
        }
        ~unique_ptr() { del_(ptr_); }
        data_t *operator->() { return ptr_; }
        data_t &operator*() { return *(ptr_); }
        data_t &operator[](const size_t num) { return *(ptr_ + num); }
    };
}

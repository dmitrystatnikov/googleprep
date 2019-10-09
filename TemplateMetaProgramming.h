#pragma once

#include <tuple>

namespace tmp
{

#pragma region Type Sequence

template <typename... ArgsT>
struct TypeSequence
{
   using type = TypeSequence<ArgsT...>;
};

template <typename... ArgsT>
struct TypeSequence< TypeSequence<ArgsT...> >
{
   using type = TypeSequence<ArgsT...>;
};

template <typename T, typename... ArgsT>
struct TypeSequence< T, TypeSequence<ArgsT...> >
{
   using type = TypeSequence<T, ArgsT...>;
};

template <typename... ArgsT1, typename... ArgsT2>
struct TypeSequence< TypeSequence<ArgsT1...>, TypeSequence<ArgsT2...> >
{
   using type = TypeSequence<ArgsT1..., ArgsT2...>;
};

#pragma endregion

#pragma region Type To Type Convertor

template <template <typename ArgT> class ConverterT, typename... ArgsT>
struct TypeToTypeConverter {};

template <template <typename ArgT> class ConverterT, typename ArgFirstT, typename... ArgsT>
struct TypeToTypeConverter <ConverterT, ArgFirstT, ArgsT...>
{
   using type = TypeSequence < TypeToTypeConverter <ConverterT, ArgFirstT>::type, TypeToTypeConverter<ConverterT, ArgsT...>::type >::type;
};

template <template <typename ArgT> class ConverterT, typename T>
struct TypeToTypeConverter <ConverterT, T>
{
   using type = ConverterT<T>;
};

#pragma endregion

#pragma region Type To Value Convertor

template <template <typename ArgT> class ConverterT, typename... ArgsT>
struct TypeToValueConverter {};

template <template <typename ArgT> class ConverterT, typename ArgFirstT, typename... ArgsT>
struct TypeToValueConverter<ConverterT, ArgFirstT, ArgsT...>
{
   static constexpr auto value = std::tuple_cat (TypeToValueConverter<ConverterT, ArgFirstT>::value, TypeToValueConverter<ConverterT, ArgsT...>::value);
};

template <template <typename ArgT> class ConverterT, typename T>
struct TypeToValueConverter<ConverterT, T>
{
   static constexpr auto value = std::make_tuple (ConverterT<T>::value);
};

#pragma endregion

}

#pragma region Tests

#include <iterator>

template <typename T> using Converter_t = std::is_same<T, std::random_access_iterator_tag>::type;

template <typename T> using Converter_v = std::is_same<T, std::random_access_iterator_tag>;

using B = tmp::TypeToTypeConverter<Converter_t, std::random_access_iterator_tag, std::input_iterator_tag, std::output_iterator_tag, std::forward_iterator_tag, std::bidirectional_iterator_tag>::type;

constexpr auto r1 = std::is_same_v <tmp::TypeSequence<std::true_type, std::false_type, std::false_type, std::false_type, std::false_type>, B>;

constexpr auto r2 = tmp::TypeToValueConverter<Converter_v, std::input_iterator_tag, std::random_access_iterator_tag, std::output_iterator_tag, std::forward_iterator_tag, std::bidirectional_iterator_tag>::value;

static_assert (r1, "");

static_assert (r2 == std::make_tuple (false, true, false, false, false), "");

#pragma endregion


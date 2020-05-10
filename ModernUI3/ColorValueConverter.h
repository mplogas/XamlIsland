#pragma once
#include "ColorValueConverter.g.h"

namespace winrt::ModernUI3::implementation
{
    struct ColorValueConverter : ColorValueConverterT<ColorValueConverter>
    {
        ColorValueConverter() = default;

        Windows::Foundation::IInspectable Convert(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language);
        Windows::Foundation::IInspectable ConvertBack(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language);
    };
}
namespace winrt::ModernUI3::factory_implementation
{
    struct ColorValueConverter : ColorValueConverterT<ColorValueConverter, implementation::ColorValueConverter>
    {
    };
}

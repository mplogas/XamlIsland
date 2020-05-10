#include "pch.h"
#include "ColorValueConverter.h"
#include "ColorValueConverter.g.cpp"
#include <ios>
#include <iomanip>
#include <sstream>

using namespace std;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::Xaml::Interop;

namespace winrt::ModernUI3::implementation
{
    IInspectable ColorValueConverter::Convert(IInspectable const& value, TypeName const& targetType, IInspectable const& parameter, hstring const& language)
    {
        auto propertyValue = value.as<IPropertyValue>();
        if (propertyValue.Type() != PropertyType::Double) {
            throw hresult_invalid_argument(L"Double value expected");
        }
        double number = unbox_value<double>(value);
        std::wostringstream s;
        s << fixed << setprecision(2) << number;
        hstring text{ s.str() };
        return box_value(text);
    }
    IInspectable ColorValueConverter::ConvertBack(IInspectable const& value, TypeName const& targetType, IInspectable const& parameter, hstring const& language)
    {
        auto propertyValue = value.as<IPropertyValue>();
        if (propertyValue.Type() != PropertyType::String) {
            throw hresult_invalid_argument(L"String value expected");
        }
        hstring text = unbox_value<hstring>(value);
        double number = _wtof(text.c_str());
        return box_value(number);
    }
}

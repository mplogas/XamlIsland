#pragma once
#include "ColorValueConverter.h"
#include "ColorPickerControl.g.h"

#pragma warning( disable : 4324 )

namespace winrt::ModernUI3::implementation
{
    struct ColorPickerControl : ColorPickerControlT<ColorPickerControl>
    {
        const float InteractiveAreaDiameter = 0.75f;
        const float Pi = 3.1415926f;
        const float SaturationCircleMinRadius = 0.2f;
        const float SaturationCircleScale = 0.8f;

        static Microsoft::UI::Xaml::DependencyProperty _rProperty;
        static Microsoft::UI::Xaml::DependencyProperty _gProperty;
        static Microsoft::UI::Xaml::DependencyProperty _bProperty;
        static Microsoft::UI::Xaml::DependencyProperty _yProperty;
        static Microsoft::UI::Xaml::DependencyProperty _headerProperty;

        /* Helper constructor functions */
        winrt::Microsoft::UI::Xaml::Media::TranslateTransform make_translate_transform(winrt::Windows::Foundation::Numerics::float2 translate);
        winrt::Microsoft::UI::Xaml::Media::CompositeTransform make_translate_scale_transform(winrt::Windows::Foundation::Numerics::float2 translate,winrt::Windows::Foundation::Numerics::float2 scale);
        inline winrt::Windows::UI::Color make_color(DirectX::XMVECTOR const& rgbColor) {
            return { (uint8_t)(255 * DirectX::XMVectorGetW(rgbColor)),(uint8_t)(255 * DirectX::XMVectorGetX(rgbColor)), (uint8_t)(255 * DirectX::XMVectorGetY(rgbColor)), (uint8_t)(255 * DirectX::XMVectorGetZ(rgbColor)) };
        }

        void UpdateColorGradientBrush(winrt::Windows::Foundation::Size const& newSize);
        void UpdatePickerPosition(winrt::Windows::Foundation::Numerics::float2 const& position);
		void UpdatePickerSize();
        void UpdateProperties();
        void UpdateUIFromRgb();

        winrt::Windows::Foundation::Numerics::float2 _middlePoint{ 0,0 };
        winrt::Windows::Foundation::Numerics::float2 _manipulationStartingPoint{ 0,0 };

        winrt::Windows::Foundation::Point _pickerPosition{ 0,0 };   // Picker location on screen relative to center point
        bool _manipulationActive{ false };
        float _interactiveAreaRadius{ 0 };
        bool _uiUpdatesSuspended{ false };
        DirectX::XMVECTOR _hsvCurrentColor = DirectX::XMVectorSet(0, 1, 0, 1);  // A = 1, Sat = 1

        ColorPickerControl();

        void InteractiveAreaSizeChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::SizeChangedEventArgs const& args);
        void InteractiveAreaTapped(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const& args);
        void OnPositionManipulationStarted(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::ManipulationStartedRoutedEventArgs const& args);
        void OnPositionManipulationDelta(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::ManipulationDeltaRoutedEventArgs const& args);
        void OnPositionManipulationCompleted(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::ManipulationCompletedRoutedEventArgs const& args);

        void OnSizeManipulationStarted(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::ManipulationStartedRoutedEventArgs const& args);
        void OnSizeManipulationDelta(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::ManipulationDeltaRoutedEventArgs const& args);
        void OnSizeManipulationCompleted(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::ManipulationCompletedRoutedEventArgs const& args);


        static void OnRGBValueChanged(Microsoft::UI::Xaml::DependencyObject const& d, Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e);
        static void OnYValueChanged(Microsoft::UI::Xaml::DependencyObject const& d, Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e);

        static Microsoft::UI::Xaml::DependencyProperty RProperty();
        static Microsoft::UI::Xaml::DependencyProperty GProperty();
        static Microsoft::UI::Xaml::DependencyProperty BProperty();
        static Microsoft::UI::Xaml::DependencyProperty YProperty();
        static Microsoft::UI::Xaml::DependencyProperty HeaderProperty();


        double R();
        void R(double value);
        double G();
        void G(double value);
        double B();
        void B(double value);
        double Y();
        void Y(double value);
        hstring Header();
        void Header(hstring value);

        void Marker_PointerEntered(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void Marker_PointerExited(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e);
    };
}

namespace winrt::ModernUI3::factory_implementation
{
    struct ColorPickerControl : ColorPickerControlT<ColorPickerControl, implementation::ColorPickerControl>
    {
    };
}

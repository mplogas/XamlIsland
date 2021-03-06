﻿#pragma once

#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "winrt/Windows.UI.Xaml.Controls.Primitives.h"
#include "ColorPickerControl.g.h"

namespace winrt::UWPControl::implementation
{
    struct ColorPickerControl : ColorPickerControlT<ColorPickerControl>
    {
        const float InteractiveAreaDiameter = 0.75f;
        const float Pi = 3.1415926f;
        const float SaturationCircleMinRadius = 0.2f;
        const float SaturationCircleScale = 0.8f;

        static Windows::UI::Xaml::DependencyProperty _rProperty;
        static Windows::UI::Xaml::DependencyProperty _gProperty;
        static Windows::UI::Xaml::DependencyProperty _bProperty;

        /* Helper constructor functions */
        winrt::Windows::UI::Xaml::Media::TranslateTransform make_translate_transform(winrt::Windows::Foundation::Numerics::float2 translate);
        winrt::Windows::UI::Xaml::Media::CompositeTransform make_translate_scale_transform(winrt::Windows::Foundation::Numerics::float2 translate, winrt::Windows::Foundation::Numerics::float2 scale);
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

        void OnSizeChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::SizeChangedEventArgs const& args);
        void OnTapped(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::Input::TappedRoutedEventArgs const& args);
        void OnPositionManipulationStarted(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::Input::ManipulationStartedRoutedEventArgs const& args);
        void OnPositionManipulationDelta(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::Input::ManipulationDeltaRoutedEventArgs const& args);
        void OnPositionManipulationCompleted(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::Input::ManipulationCompletedRoutedEventArgs const& args);

        void OnSizeManipulationStarted(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::Input::ManipulationStartedRoutedEventArgs const& args);
        void OnSizeManipulationDelta(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::Input::ManipulationDeltaRoutedEventArgs const& args);
        void OnSizeManipulationCompleted(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::Input::ManipulationCompletedRoutedEventArgs const& args);


        static void OnRGBValueChanged(Windows::UI::Xaml::DependencyObject const& d, Windows::UI::Xaml::DependencyPropertyChangedEventArgs const& e);

        static Windows::UI::Xaml::DependencyProperty RProperty();
        static Windows::UI::Xaml::DependencyProperty GProperty();
        static Windows::UI::Xaml::DependencyProperty BProperty();

        double R();
        void R(double value);
        double G();
        void G(double value);
        double B();
        void B(double value);
        void Marker_PointerEntered(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void Marker_PointerExited(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e);
    };
}

namespace winrt::UWPControl::factory_implementation
{
    struct ColorPickerControl : ColorPickerControlT<ColorPickerControl, implementation::ColorPickerControl>
    {
    };
}

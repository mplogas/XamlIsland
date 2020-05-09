#include "pch.h"
#include "ColorPickerControl.h"

#if __has_include("ColorPickerControl.g.cpp")
#include "ColorPickerControl.g.cpp"
#endif

#include <DirectXMath.h>
#include <algorithm>

using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Numerics;

using namespace winrt::Microsoft::UI;
using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Input;
using namespace winrt::Microsoft::UI::Xaml::Media;
using namespace winrt::Microsoft::UI::Xaml::Media::Imaging;
using namespace winrt::Microsoft::UI::Xaml::Shapes;

#include <winrt/Windows.UI.Xaml.Interop.h>

using namespace DirectX;

namespace winrt::ModernUI3::implementation
{
	Microsoft::UI::Xaml::DependencyProperty ColorPickerControl::_rProperty =
		Microsoft::UI::Xaml::DependencyProperty::Register(
			L"R",
			winrt::xaml_typename<double>(),
			winrt::xaml_typename<ModernUI3::ColorPickerControl>(),
			Microsoft::UI::Xaml::PropertyMetadata{ winrt::box_value(0.0), 
			Microsoft::UI::Xaml::PropertyChangedCallback{ &ColorPickerControl::OnRGBValueChanged } }
	);
	Microsoft::UI::Xaml::DependencyProperty ColorPickerControl::_gProperty =
		Microsoft::UI::Xaml::DependencyProperty::Register(
			L"G",
			winrt::xaml_typename<double>(),
			winrt::xaml_typename<ModernUI3::ColorPickerControl>(),
			Microsoft::UI::Xaml::PropertyMetadata{ winrt::box_value(0.0),
			Microsoft::UI::Xaml::PropertyChangedCallback{ &ColorPickerControl::OnRGBValueChanged } }
	);
	Microsoft::UI::Xaml::DependencyProperty ColorPickerControl::_bProperty =
		Microsoft::UI::Xaml::DependencyProperty::Register(
			L"B",
			winrt::xaml_typename<double>(),
			winrt::xaml_typename<ModernUI3::ColorPickerControl>(),
			Microsoft::UI::Xaml::PropertyMetadata{ winrt::box_value(0.0),
			Microsoft::UI::Xaml::PropertyChangedCallback{ &ColorPickerControl::OnRGBValueChanged } }
	);

	ColorPickerControl::ColorPickerControl()
	{		
		InitializeComponent();
		interactiveArea().SizeChanged({ this,&ColorPickerControl::InteractiveAreaSizeChanged });
		interactiveArea().Tapped({ this,&ColorPickerControl::InteractiveAreaTapped });
		pickerPositionMarker().ManipulationMode(ManipulationModes::TranslateX | ManipulationModes::TranslateY);
		pickerPositionMarker().ManipulationStarted({this,&ColorPickerControl::OnPositionManipulationStarted});
		pickerPositionMarker().ManipulationDelta({ this,&ColorPickerControl::OnPositionManipulationDelta });
		pickerPositionMarker().ManipulationCompleted({this,&ColorPickerControl::OnPositionManipulationCompleted});

		pickerSizeMarker().ManipulationMode(ManipulationModes::TranslateX | ManipulationModes::TranslateY);
		pickerSizeMarker().ManipulationStarted({this,&ColorPickerControl::OnSizeManipulationStarted});
		pickerSizeMarker().ManipulationDelta({ this,&ColorPickerControl::OnSizeManipulationDelta });
		pickerSizeMarker().ManipulationCompleted({this, &ColorPickerControl::OnSizeManipulationCompleted});
	}

	void ColorPickerControl::InteractiveAreaSizeChanged(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::SizeChangedEventArgs const& args)
	{
		float halfSize = (float) std::min(args.NewSize().Width, args.NewSize().Height) / 2;
		_middlePoint = { args.NewSize().Width / 2, args.NewSize().Height / 2 };
		_interactiveAreaRadius = (float) halfSize * InteractiveAreaDiameter;

		CompositeTransform translateAndScale = make_translate_scale_transform(_middlePoint, float2 { _interactiveAreaRadius });
		
		TranslateTransform translate = make_translate_transform(_middlePoint);
		
		crossHair().Data().Transform(translateAndScale);
		pickerCircle().Data().Transform(translateAndScale);
		pickerPositionMarker().Data().Transform(translate);
		//secondMarkerCircle().Data().Transform(translateAndScale);
		colorLine().Data().Transform(translateAndScale);

		auto lineRenderTransform = colorLine().RenderTransform().as<CompositeTransform>();
		lineRenderTransform.CenterX(_middlePoint.x);
		lineRenderTransform.CenterY(_middlePoint.y);

		CompositeTransform scaleFullSize = make_translate_scale_transform(_middlePoint, float2{ halfSize });

		colorGradient().Data().Transform(scaleFullSize);

		UpdateColorGradientBrush({ (float) halfSize * 2, (float) halfSize * 2 });
	
		UpdatePickerPosition(_pickerPosition);
		UpdatePickerSize();
	}

	/* When tapped update the color picker location if that is within the interactive area */
	void ColorPickerControl::InteractiveAreaTapped(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const& args)
	{
		auto tapPosition = args.GetPosition(sender.as<UIElement>());
		if (distance(tapPosition,_middlePoint) < _interactiveAreaRadius) {
			UpdatePickerPosition(tapPosition - _middlePoint);
			UpdateProperties();
		}
	}

	void ColorPickerControl::OnPositionManipulationStarted(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::ManipulationStartedRoutedEventArgs const&)
	{
		_manipulationStartingPoint = _pickerPosition;
		_manipulationActive = true;
	}

	void ColorPickerControl::OnPositionManipulationDelta(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::ManipulationDeltaRoutedEventArgs const& args)
	{
		auto newPosition = _manipulationStartingPoint + (float2) args.Cumulative().Translation;

		// Limit position to the interactivity circle
		if (length(newPosition) > _interactiveAreaRadius) {
			float angle = atan2(newPosition.y, newPosition.x);
			newPosition = { _interactiveAreaRadius * cos(angle), _interactiveAreaRadius * sin(angle) };
		}

		UpdatePickerPosition(newPosition);
		UpdateProperties();
	}

	void ColorPickerControl::OnPositionManipulationCompleted(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::ManipulationCompletedRoutedEventArgs const&)
	{
		_manipulationActive = false;
	}

	void ColorPickerControl::OnSizeManipulationStarted(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::ManipulationStartedRoutedEventArgs const&)
	{
		_manipulationActive = true;

		float angle = Pi * (XMVectorGetX(_hsvCurrentColor) - 0.5f);
		float saturation = XMVectorGetY(_hsvCurrentColor);
		float circleRadius = _interactiveAreaRadius * ((1 - saturation) * SaturationCircleScale + SaturationCircleMinRadius);
		_manipulationStartingPoint = { circleRadius * cos(angle), circleRadius * sin(angle) };
	}

	void ColorPickerControl::OnSizeManipulationDelta(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::ManipulationDeltaRoutedEventArgs const& args)
	{
		float circleRelativeSize = length(args.Cumulative().Translation + _manipulationStartingPoint) / _interactiveAreaRadius;
		float saturation = 1 - std::clamp((circleRelativeSize - SaturationCircleMinRadius) / SaturationCircleScale, 0.0f, 1.0f);
		_hsvCurrentColor = XMVectorSetY(_hsvCurrentColor, saturation);
		UpdatePickerSize();
		UpdateProperties();
	}

	void ColorPickerControl::OnSizeManipulationCompleted(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::ManipulationCompletedRoutedEventArgs const&)
	{
		_manipulationActive = false;
	}

	void ColorPickerControl::Marker_PointerEntered(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const&)
	{
		auto path = sender.try_as<Path>();
		if (!_manipulationActive && path) {
			path.Fill(SolidColorBrush(Colors::White()));
		}
	}


	void ColorPickerControl::Marker_PointerExited(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const&)
	{
		auto path = sender.try_as<Path>();
		if (!_manipulationActive && path) {
			path.Fill(SolidColorBrush(Colors::Black()));
		}
	}

	/* Move picker to absolute position */
	void ColorPickerControl::UpdatePickerPosition(winrt::Windows::Foundation::Numerics::float2 const& position)
	{
		using namespace DirectX;

		_pickerPosition = position;
		auto translateTransform = make_translate_transform(position);
		pickerCircle().RenderTransform(translateTransform);
		pickerPositionMarker().RenderTransform(translateTransform);
		
		float hue = 0.5f + -atan2(position.x,position.y) / (2 * Pi);	// Color wheel is rotated by pi/2 on screen, swap x & y and -angle
		float value = length(position) / _interactiveAreaRadius;

		_hsvCurrentColor = XMVectorSet(hue, XMVectorGetY(_hsvCurrentColor), value, XMVectorGetW(_hsvCurrentColor));

		XMVECTOR rgbColor = XMColorHSVToRGB(_hsvCurrentColor);

		auto lineTransform = colorLine().RenderTransform().as<CompositeTransform>();
		float lineRotation = 180 * atan2(position.y, position.x) / Pi;
		lineTransform.Rotation(lineRotation);
		lineTransform.ScaleX(value);
		lineTransform.ScaleY(value);
		auto colorLineBrush = colorLine().Stroke().as<LinearGradientBrush>();

		colorLineBrush.GradientStops().GetAt(1).Color(make_color(rgbColor));

		CompositeTransform sizePickerTransform = CompositeTransform();
		sizePickerTransform.TranslateX((double)(_middlePoint.x + position.x));
		sizePickerTransform.TranslateY((double) (_middlePoint.y + position.y));
		sizePickerTransform.Rotation((double)(lineRotation-180));

		pickerSizeMarker().RenderTransform(sizePickerTransform);
	}

	// Set picker circle size from current HSV
	void ColorPickerControl::UpdatePickerSize()
	{
		float saturation = XMVectorGetY(_hsvCurrentColor);
		float circleSize = (1 - saturation) * SaturationCircleScale + SaturationCircleMinRadius;

		auto circleGeometry = pickerCircle().Data().as<EllipseGeometry>();
		circleGeometry.RadiusX(circleSize);
		circleGeometry.RadiusY(circleSize);

		pickerSizeMarker().Data().Transform().as<CompositeTransform>().TranslateX((double) (circleSize * _interactiveAreaRadius));
	}

	void ColorPickerControl::UpdateProperties()
	{
		auto rgbColor = XMColorHSVToRGB(_hsvCurrentColor);
		_uiUpdatesSuspended = true;	// MAke sure these updates do not trigger another MovePickerTo
		R(XMVectorGetX(rgbColor));
		G(XMVectorGetY(rgbColor));
		B(XMVectorGetZ(rgbColor));
		_uiUpdatesSuspended = false;
	}

	void ColorPickerControl::UpdateUIFromRgb()
	{
		using namespace DirectX;
		if (!_uiUpdatesSuspended) {
			_hsvCurrentColor = XMColorRGBToHSV(XMVectorSet((float) R(), (float) G(), (float) B(),1.0f));
			float angle = 2 * Pi * (XMVectorGetX(_hsvCurrentColor)-0.25f);
			float length = _interactiveAreaRadius * XMVectorGetZ(_hsvCurrentColor);
			UpdatePickerPosition( { length * cos(angle), length * sin(angle) });
			UpdatePickerSize();
		}
	}

	
	void ColorPickerControl::OnRGBValueChanged(Microsoft::UI::Xaml::DependencyObject const& d, Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const&)
	{
		using namespace DirectX;
		if (ModernUI3::ColorPickerControl picker{ d.try_as<ModernUI3::ColorPickerControl>() })
		{
			auto control { get_self<ColorPickerControl>(picker) };
			control->UpdateUIFromRgb();
		}
	}
	
	double ColorPickerControl::R()
	{
		return unbox_value<double>(GetValue(_rProperty));
	}
	void ColorPickerControl::R(double value)
	{
		SetValue(_rProperty, box_value<double>(value));
	}
	double ColorPickerControl::G()
	{
		return unbox_value<double>(GetValue(_gProperty));
	}
	void ColorPickerControl::G(double value)
	{
		SetValue(_gProperty, box_value<double>(value));
	}
	double ColorPickerControl::B()
	{
		return unbox_value<double>(GetValue(_bProperty));
	}
	void ColorPickerControl::B(double value)
	{
		SetValue(_bProperty, box_value<double>(value));
	}

	Microsoft::UI::Xaml::DependencyProperty ColorPickerControl::RProperty()
	{
		return _rProperty;
	}

	Microsoft::UI::Xaml::DependencyProperty ColorPickerControl::GProperty()
	{
		return _gProperty;
	}

	Microsoft::UI::Xaml::DependencyProperty ColorPickerControl::BProperty()
	{
		return _bProperty;
	}

	winrt::Microsoft::UI::Xaml::Media::TranslateTransform ColorPickerControl::make_translate_transform(winrt::Windows::Foundation::Numerics::float2 translate)
	{
		auto transform = TranslateTransform();
		transform.X(translate.x);
		transform.Y(translate.y);
		return transform;
	}

	winrt::Microsoft::UI::Xaml::Media::CompositeTransform ColorPickerControl::make_translate_scale_transform(winrt::Windows::Foundation::Numerics::float2 translate, winrt::Windows::Foundation::Numerics::float2 scale)
	{
		auto transform = CompositeTransform();
		transform.TranslateX(translate.x);
		transform.TranslateY(translate.y);
		transform.ScaleX(scale.x);
		transform.ScaleY(scale.y);
		return transform;
	}

	void ColorPickerControl::UpdateColorGradientBrush(winrt::Windows::Foundation::Size const& newSize)
	{
		using namespace DirectX;
		int32_t width = (int32_t)newSize.Width;
		int32_t height = (int32_t)newSize.Height;
		float2 halfSize{ newSize.Width / 2, newSize.Height / 2 };

		auto bitmap = WriteableBitmap(width, height);
		uint8_t* pBitmapData = bitmap.PixelBuffer().data();

		// Use vectrs for fast math, calculate 4 points at once
		XMVECTOR centerPointX = XMVectorReplicate(newSize.Width / 2.0f);
		XMVECTOR centerPointY = XMVectorReplicate(newSize.Height / 2.0f);

		XMVECTOR vStep = XMVectorReplicate(4);

		struct BGRA {
			uint8_t B;
			uint8_t G;
			uint8_t R;
			uint8_t A;
		};

		BGRA* pRowData = (BGRA*)pBitmapData;

		for (int32_t y = 0; y < height; y++, pRowData += width)
		{
			XMVECTOR vPosX = XMVectorSet(0, 1, 2, 3);
			XMVECTOR vPosY = XMVectorReplicate((float)y);

			BGRA* pBGRA = pRowData;
			for (int32_t x = 0; x < width; x += 4, vPosX += vStep)
			{
				XMVECTOR vectorX = XMVectorSubtract(centerPointX, vPosX);
				XMVECTOR vectorY = XMVectorSubtract(centerPointY, vPosY);

				// Relative angle to the center point from 0 to 1
				XMVECTOR angle = XMVectorAddAngles(XMVectorATan2Est(vectorY, vectorX), g_XMHalfPi);

				// As bitmap size might not be multiple of 4 we need to copy the values with width check like this
				for (int bitmapX = 0; bitmapX < 4 && x + bitmapX < width; bitmapX++, pBGRA++)
				{
					// Set angle to Hue, Sat = 1, Value to 0 and Alpha to 0
					XMVECTOR hsv = XMVectorSet(0.5f + XMVectorGetByIndex(angle, bitmapX) / (2.0f * Pi), 1.0f, 1.0f, 1.0f);
					XMVECTOR rgb = XMColorHSVToRGB(hsv);
					pBGRA->B = (uint8_t)(XMVectorGetZ(rgb) * 255);
					pBGRA->G = (uint8_t)(XMVectorGetY(rgb) * 255);
					pBGRA->R = (uint8_t)(XMVectorGetX(rgb) * 255);
					pBGRA->A = 255;
				}
			}
		}

		colorGradient().Fill().as<ImageBrush>().ImageSource(bitmap);
	}

}




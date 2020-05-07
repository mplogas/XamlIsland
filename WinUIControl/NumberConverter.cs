using Microsoft.UI.Xaml.Data;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace WinUIControl
{
    public sealed class NumberConverter : IValueConverter
    {
        public object Convert(object input, Type targetType, object parameter, string language)
        {
            var result = Math.Round((double)input * 255, 0);
            return result.ToString();
        }

        public object ConvertBack(object input, Type targetType, object parameter, string language)
        {
            if (int.TryParse((string)input, out int i))
            {
                return (double)(i / 255);
            }
            else return (double)0;

        }
    }
}

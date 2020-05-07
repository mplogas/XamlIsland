using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace WinUIControl
{
    public sealed class ColorPickerViewModel : INotifyPropertyChanged
    {
        private double r;
        public double R { get => this.r; set => SetProperty(ref this.r, value, nameof(R)); }

        private double g;
        public double G { get => this.g; set => SetProperty(ref this.g, value, nameof(G)); }

        private double b;
        public double B { get => this.b; set => SetProperty(ref this.b, value, nameof(B)); }

        private double y;
        public double Y { get => this.y; set => SetProperty(ref this.y, value, nameof(Y)); }

        #region INotifyPropertyChanged
        private bool SetProperty<T>(ref T backingStore, T value,
            [CallerMemberName] string propertyName = "",
            Action onChanged = null)
        {
            if (EqualityComparer<T>.Default.Equals(backingStore, value))
                return false;

            backingStore = value;
            onChanged?.Invoke();
            OnPropertyChanged(propertyName);
            return true;
        }

        public event PropertyChangedEventHandler PropertyChanged;
        private void OnPropertyChanged([CallerMemberName] string propertyName = "")
        {
            var changed = PropertyChanged;
            changed?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
        #endregion
    }
}

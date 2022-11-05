import QtQuick 2.15
import QtQuick.Controls 2.15

SpinBox {
    property real realStepSize: 0.1
    property real min: 0
    property real max: 100
    property int digits: 3
    property int k: Math.pow(10, digits)

    from: k * min
    to: k * max
    stepSize: k * realStepSize
    editable: true

    property real realValue: value / k
    value: 1000

    validator: DoubleValidator {
        bottom: Math.min(from, to)
        top:  Math.max(from, to)
    }

    textFromValue: function(value, locale) {
        return Number(value / k).toLocaleString(locale, 'f', digits)
    }

    valueFromText: function(text, locale) {
        return Number.fromLocaleString(locale, text) * k
    }
}

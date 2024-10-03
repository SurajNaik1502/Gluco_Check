import serial
from flask import Flask, jsonify

# STM32 ke saath serial connection setup karo (adjust COM port and baud rate accordingly)
ser = serial.Serial('/dev/ttyUSB0', 115200)  # Use appropriate port

# ser = serial.Serial('/dev/ttyACM0', 115200)  # Mac/Linux ke liye
# ser = serial.Serial('COM3', 115200)  # Windows ke liye 
# ser = serial.Serial('COM4', 115200)  # Windows use ke liye

app = Flask(__name__)

@app.route('/glucose', methods=['GET'])
def get_glucose_level():
    if ser.in_waiting > 0:
        glucose_data = ser.readline().decode('utf-8').strip()
        return jsonify({'glucose_level': glucose_data})
    return jsonify({'error': 'No data available'}), 404

if __name__ == '__main__':
    app.run(debug=True, port=5000)


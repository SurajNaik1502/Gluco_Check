import serial
from flask import Flask, jsonify
import time
from flask_cors import CORS

app = Flask(_name_)
CORS(app)

# Initialize serial connection
def initialize_serial(port='COM9', baudrate=115200, timeout=1):
    try:
        ser = serial.Serial(port, baudrate, timeout=timeout)
        time.sleep(2)  # Wait for the connection to initialize
        print(f"Serial connection established on {port}.")
        return ser
    except serial.SerialException as e:
        print(f"Error connecting to serial port {port}: {e}")
        return None

# Initialize serial connection (adjust 'COM9' as needed)
ser = initialize_serial('COM9')

@app.route('/glucose', methods=['GET'])
def get_glucose_level():
    if ser and ser.is_open:
        try:
            if ser.in_waiting > 0:
                glucose_data = ser.readline().decode('utf-8').strip()
                return jsonify({'glucose_level': glucose_data})
            else:
                return jsonify({'error': 'No data available'}), 404
        except Exception as e:
            return jsonify({'error': f'Failed to read data: {str(e)}'}), 500
    else:
        return jsonify({'error': 'Serial port not connected'}), 500

if _name_ == '_main_':
    app.run(debug=True, port=5000, use_reloader=False)
from flask import Flask, request, Response, jsonify
import datetime
import threading
import ssl

app = Flask(__name__)

# --- SHARED LOGIC ---
@app.before_request
def log_request_info():
    protocol = "HTTPS" if request.is_secure else "HTTP"
    print(f"\n[{datetime.datetime.now().strftime('%H:%M:%S')}] {protocol} {request.method} to {request.host}")
    print(f"Path: {request.path}")
    if request.headers.get('apikeyheader'):
        print(f"API-Key found: {request.headers.get('apikeyheader')[:10]}...")

# --- ENDPOINT: Bose Auth (typically HTTPS) ---
@app.route('/v1/auth', methods=['GET'])
def bose_auth():
    print(f"[!] Handling Bose Auth Check.")

    # Create response object
    res = Response("", status=200)
    # 1. Clear default headers if necessary and set specific ones
    res.headers['Connection'] = 'keep-alive'
    res.headers['Host'] = 'audionotificationdev.api.bosecm.com'
    res.headers['X-Forwarded-For'] = '127.0.0.1' # You can spoof this
    res.headers['X-Forwarded-Port'] = '443'
    res.headers['X-Forwarded-Proto'] = 'https'
    res.headers['User-Agent'] = 'Bose_Lisa/27.0.6'
    res.headers['Accept'] = '*/*'
    res.headers['apikeyheader'] = request.headers.get('apikeyheader', 'Ml7YGAI9JWjFhU7D348e86JPXtisddBa')
    return res

# --- ENDPOINT: iHeart Country Check (typically HTTP) ---
@app.route('/api/v1/account/getCountry', methods=['GET'])
def iheart_country():
    partner_id = request.args.get('partnerId', 'Unknown')
    print(f"[!] Processing iHeartRadio Country Check for: {partner_id}")
    return jsonify({"countryCode": "DE"})

# --- CATCH-ALL: Discovery Mode ---
@app.route('/', defaults={'path': ''})
@app.route('/<path:path>')
def catch_all(path):
    print(f"[?] New Discovery: {request.host}{request.path}.")
    return jsonify({"status": "captured", "path": path}), 200

# --- SERVER RUNNERS ---

def run_http_server():
    print("[*] Starting HTTP Instance on Port 8080")
    app.run(host='0.0.0.0', port=8080, debug=False, threaded=True)

def run_https_server():
    print("[*] Starting HTTPS Instance on Port 8443")
    # SSL Context setup
    context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
    context.load_cert_chain(certfile="cert.pem", keyfile="key.pem")
    app.run(host='0.0.0.0', port=8443, debug=False, threaded=True, ssl_context=context)

if __name__ == '__main__':
    # Start both servers in separate threads
    http_thread = threading.Thread(target=run_http_server)
    https_thread = threading.Thread(target=run_https_server)

    http_thread.start()
    https_thread.start()

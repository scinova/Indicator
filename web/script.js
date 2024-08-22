var _device;
var _server;
var _service;
var _commandCh;
var _statusCh;

var COMMAND_UUID = "f9a4c5f3-e63e-426a-abbc-cf9fcce71a5a";
var SERVICE_UUID = "f725ca05-413c-4352-9e90-bdd8b917c3d0";
var STATUS_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8";

$(init);

function command(cmd) {
	if (!_device || !_device.gatt.connected)
		return
	var val = Uint8Array.of(cmd);
	_commandCh.writeValue(val);
}

function updateIndicator(name, state) {
	var url = 'gfx/';
	if (state)
		$('#img' + name).attr('src', url + name + '-on.png');
	else
		$('#img' + name).attr('src', url + name + '-off.png');
}

function loop() {
	if (_device && _device.gatt.connected)
		updateIndicator('Bluetooth', true);
	else
		updateIndicator('Bluetooth', false);
	setTimeout(loop, 1000);
}

function handleNotifications(event) {
	let value = event.target.value;
	var bits = value.getUint8();
	updateIndicator('PositionLights', bits & 1);
	updateIndicator('LowBeamLights', bits & 2);
	updateIndicator('HighBeamLights', bits & 4);
	updateIndicator('LeftTurn', bits & 8);
	updateIndicator('RightTurn', bits & 16);
	updateIndicator('Hazard', bits & 32);
}

function log(text) {
	$('#log').html(text + '<br />' + $('#log').html());
}

function bluetoothClick() {
	if (!_device || (_device && !_device.gatt.connected))
		connect();
	else
		disconnect();
}

function connect() {
	log('connect');
	var options = {
		acceptAllDevices: true,
		optionalServices: [SERVICE_UUID]
	};
	navigator.bluetooth.requestDevice(options)
	.then(device => {
		_device = device;
		return device.gatt.connect();
	})
	.then(server => {
		_server = server;
		setupServices();
	})
	.catch(error => { log(error); });
}

function disconnect() {
	if (_device && _device.gatt.connected)
		try {
			_device.gatt.disconnect();
		} catch(error) {
			log(error);
		}
}

function setupServices() {
	let service = _server.getPrimaryService(SERVICE_UUID);
	service.then(service => {
		_service = service;
		let statusCh = _service.getCharacteristic(STATUS_UUID);
		statusCh.then(ch => {
			_statusCh = ch;
			_statusCh.addEventListener('characteristicvaluechanged', handleNotifications);
			_statusCh.startNotifications();
		});
		let commandCh = _service.getCharacteristic(COMMAND_UUID);
		commandCh.then(ch => {
			_commandCh = ch;
		});
	});
}

function init() {
	updateIndicator('Bluetooth', false);
	updateIndicator('PositionLights', false);
	updateIndicator('LowBeamLights', false);
	updateIndicator('HighBeamLights', false);
	updateIndicator('LeftTurn', false);
	updateIndicator('RightTurn', false);
	updateIndicator('Hazard', false);
	loop();
}

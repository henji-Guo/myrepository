<template>
	<view>
		<button type="primary"  plain="true" @click="connect">mqtt 连接</button>
		<button type="primary"  plain="true" @click="subscribe">mqtt 订阅</button>
		<button type="primary"  plain="true" @click="TurnON">开灯</button>
		<button type="primary"  plain="true" @click="TurnOFF">关灯</button>
		<button type="primary"  plain="true" @click="unsubscribe">取消订阅</button>
		<button type="primary"  plain="true" @click="unconnect">断开连接</button>
		<view>message:{{ receiveMessage.toString() }}</view>
	</view>
</template>

<script>
	import mqtt from '../../utils/mqtt.js';//二选一
	//import mqtt from '../../utils/mqtt.min.js';
	export default {
		data() {
			return {
				serve: {
					host: '47.95.249.141',
					port: '8083',
					path: '/mqtt',
				},
				onTopic: '/LED',
				Qos: 0,
				sendMassage: 'Hello MQTT',
				time:0,
				receiveMessage: '',
				client: null,
				//MQTT连接的配置
				options: {
					wsOptions: {},
					protocolVersion: 4, //MQTT连接协议版本
					clientId: 'MQTT-test',
					keepalive: 60,
					clean: false,
					username: 'admin',
					password: 'public',
					reconnectPeriod: 1000, //1000毫秒，两次重新连接之间的间隔
					connectTimeout: 30 * 1000, //1000毫秒，两次重新连接之间的间隔
					resubscribe: true //如果连接断开并重新连接，则会再次自动订阅已订阅的主题（默认true）
				},
			};
		},
		methods: {
			
			connect: function() {
				var hosts = '',
				// #ifdef H5
				hosts = 'ws://' + this.serve.host + ':' + this.serve.port + this.serve.path
				//#endif
				// #ifdef MP-WEIXIN 
				hosts = 'wxs://' + this.serve.host + this.serve.path
				//#endif
				// #ifdef APP-PLUS
				hosts = 'wx://' + this.serve.host + ':' + this.serve.port + this.serve.path
				//#endif

				if (this.client == null || this.client.connented == false) {
					uni.showLoading({
						title: '连接中···'
					})
					console.log(hosts, this.options)
					this.client = mqtt.connect(
						hosts,
						this.options
					);

					this.client.on('connect', () => {
						uni.hideLoading();
						this.showToast('连接成功', 1000, 'success')
					});
					this.client.on('message', (topic, message) => {
						console.log('收到来自' + topic + '的消息' + message.toString());
						this.receiveMessage = message

					});
				}

				this.client.on('reconnect', error => {
					uni.hideLoading();
					this.showToast('正在重连···', 1000)

				});
				this.client.on('error', error => {
					uni.hideLoading();
					this.showToast('连接失败!', 1000)
				});

			},
			subscribe: function() {
				// 判断是否已成功连接
				if (!this.client || !this.client.connected) {
					this.showToast('客户端未连接', 1000)
					return;
				}

				this.client.subscribe(this.onTopic, {
					qos: this.Qos
				}, error => {
					if (!error) {
						this.showToast('订阅成功', 1000, 'success')
						console.log('订阅成功');
					}
				});

				/* //订阅多个主题
			this.client.subscribe(['one', 'two', 'three'], { qos: 1 }, err => {
				console.log(err || '订阅成功');
				this.show(err || '订阅成功');
			 });
			
				    // 订阅不同 qos 的不同主题
				    this.client.subscribe(
				        [
				            { hello: 1 },
				            { 'one/two/three': 2 },
				            { '#': 0 }
				        ],
				        (err) => {
				          this.show();console.log(err || '订阅成功')
				        },
				    )
		
		
			}); */
			},
			publish: function() {
				// 判断是否已成功连接
				if (!this.client || !this.client.connected) {
					this.showToast('客户端未连接', 1000)
					return;
				}
				if (this.sendMassage != '') {
					var send=''
					send=this.sendMassage+(this.time++)
					this.client.publish(this.onTopic,send, error => {
						console.log(error || '消息发布成功');
						this.showToast('消息发布成功', 1000, 'success')
					});
				} else {
					this.showToast('发布消息为空', 1000)
				}

			},
			TurnON: function() {
				// 判断是否已成功连接
				if (!this.client || !this.client.connected) {
					this.showToast('客户端未连接', 1000)
					return;
				}
				//if (this.sendMassage != '') {
					var send= "{\"LED_ON\": true}"
					//send=this.sendMassage+(this.time++)
					this.client.publish(this.onTopic,send, error => {
						console.log(error || '开灯');
						this.showToast('开灯成功', 1000, 'success')
					});
				//} else {
				//	this.showToast('发布消息为空', 1000)
				//}
			
			},
			TurnOFF: function() {
				// 判断是否已成功连接
				if (!this.client || !this.client.connected) {
					this.showToast('客户端未连接', 1000)
					return;
				}
				//if (this.sendMassage != '') {
					var send= "{\"LED_ON\": false}"
					//send=this.sendMassage+(this.time++)
					this.client.publish(this.onTopic,send, error => {
						console.log(error || '关灯');
						this.showToast('关灯成功', 1000, 'success')
					});
				//} else {
				//	this.showToast('发布消息为空', 1000)
				//}
			
			},
			unsubscribe: function() {
				this.client.unsubscribe(
					// topic, topic Array, topic Array-Onject
					// ['one', 'two', 'three'],
					this.onTopic,
					err => {
						console.log(err || '取消订阅成功');
						this.showToast('取消订阅成功', 1000, 'success')
					}
				);
			},
			unconnect: function() {
				this.client.end();
				this.client = null
				this.showToast('成功断开连接', 1000, 'success')
				console.log('断开连接');
			},
			showToast: function(title, time, icon = 'none') {
				uni.showToast({
					title: title,
					icon: icon,
				});
				setTimeout(function() {
					uni.hideToast();
				}, time);
			},
		}
	}
</script>
<style>
	button {
		margin-top: 30upx;
		margin-bottom: 30upx;
	}
</style>

#include <stdio.h>
#include <string.h>
#include <mosquitto.h>

#define AUTHOR "ENES_ALADAG"

/*
ONEMLI NOT: rc degiskenini basta socket baglantısını test etmek üzere olarak atıyoruz
bağlantı sağlanabilir fakat bağlantı sağlanması doğru bir şekilde veri iletimi olacağı 
anlamına gelmez. CONNECT/CONNACK bağlantısı sağlanmalıdır. Bundan ötürü on_connect methodu 
tanımlanmıştır.
*/

void on_publish(struct mosquitto *mosq, void *obj, int mid)
{
	printf("Mesaj %d yayınlandı.\n", mid);
}

void on_connect(struct mosquitto *mosq, void *obj, int reason_code)
{
	printf("on_connect: %s\n", mosquitto_connack_string(reason_code));
	if(reason_code != 0){
		/*Eğer bağlantı herhangi bir sebepten ötürü sağlanamaz ise bağlantı kesiliyor.
		sürekli bağlantı sağlamak gerekirse döngü olarak bağlantı testi tanımlanmalı ve 0 döndürmeli
		0==True::*/
		mosquitto_disconnect(mosq);
	}
}



int send_message(char *payload){
	mosquitto_lib_init();
	struct mosquitto *mosq; //mosquitto.h içerisindeki mosquitto yapısına mosq adında pointer atandı.
	mosq=mosquitto_new(NULL,true,NULL);//isimsiz bir yapı oluşturuldu
	/*39. ve 40. satırlar bağlantının doğru bir şekilde başlatılabilmesi için 
	büyük önem taşıyor ilk kısımda bahsedilen CONNECT/CONNACK doğrulaması sağlanıyor.*/
    mosquitto_connect_callback_set(mosq, on_connect);
	mosquitto_publish_callback_set(mosq, on_publish);

    int rc;
    rc=mosquitto_connect(mosq,"localhost",1883,60);//socket bağlantı testi

    	if (rc!=0){
		printf("Bağlantı sağlanamadı");
		mosquitto_destroy(mosq);
		return -1;
	}
	/*bağlantı sürekliliği için mosquitto kütüphanesi içerisindeki döngüyü başlatıyoruz.*/
    rc=mosquitto_loop_start(mosq);
		

	/*Bağlantı döngümüz çalışmadığı takdirde dönen 1 olacaktır daha sonra gambas içerisinde koşullandırma
	yapılmalı.*/
    if(rc != MOSQ_ERR_SUCCESS){
		mosquitto_destroy(mosq);
		fprintf(stderr, "Hata: %s\n", mosquitto_strerror(rc));
		return 1;
	}





	printf("Mesaj gönderimi başlatılıyor...");
	/*Test aşamasında paket yoğunluğunu analiz edebilmek için sonsuz döngüye atandı
	ar-ge ortamından çıkışında ağ stabilliğine göre gecikme atanacak.
	*/
	while(true){
	mosquitto_publish(mosq,NULL,"targe/main",strlen(payload),payload,0,false);
	}
	return 0;
	
}


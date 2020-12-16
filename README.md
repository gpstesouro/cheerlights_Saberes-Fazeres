## cheerlights_Saberes-Afazeres
***

Explorando o [@CheerLights](https://cheerlights.com/) como meio de instrução para a programação da placa ESP32 Heltec V2. 

Primeiro um pouquinho de história!
CheerLights é um projeto de “Internet das Coisas” criado por Hans Scharler que permite que as luzes (lâmpadas, fitas ou cordões de leds) de pessoas em todo o mundo sejam sincronizadas com uma cor sugerida por meio de uma mensagem pelo Twitter. Esta é uma das formas de conectar/comandar coisas físicas ([arranjos luminosos de Natal](https://cheerlights.com/live/), displays, painéis ou aplicativos) por intermédio de redes sociais, no caso em voga, [Twitter](https://twitter.com/).

Experimente, você mesmo, enviando um tweet. Simplesmente digite: **@CheerLights red now** <enter>

Essa mensagem será processada e causará uma reação em cadeia, fazendo com que todos os dispositivos "CheerLights" mudem de cor para vermelho, inclusive o painel de leds ao lado da minha mesa. As cores podem ser: *red, pink, green, blue, cyan, white, warmwhite, oldlace, purple, magenta, yellow ou orange*. Escolha e mande bala, ou melhor, pode *tweetar* à vontade.

***

Muito bem e para que serve isto? Por qual motivo terei interesse em mudar a cor da árvore de Natal do vizinho, mesmo sabendo que este "vizinho invisível" poderá estar do outro lado do planeta? E daí ...

Bem, o CheerLights é capaz de proporcionar um ambiente fértil para explorar um série de recursos de programação, a depender de sua imaginação e criatividade. Uma vez se divertido, sem  compromisso, com o CheerLights; fica mais simples e fácil transpor o conhecimento adquirido para uma aplicação prática. **Inspirá-lo**, será o objetivo do *sketch* apresentado, o qual abordará:
***
- [ ] v.1: vamos lá, sei que você não se contentará com a monotonia de uma única cor (monocromático). Façamos dois registros históricos: (i) um visual (fita de leds) com os 48 últimos registros de cores amostrados (discretizados) a cada 10 segundos; e (ii) prover dados para um banco de dados com a cor e o respectivo intervalo de permanência da cor inalterada;  
- [ ] v.1.1: display do ESP32 com a indicação da cor e tempo de exposição da respectiva cor; e
- [ ] v.1.2: captação de um idDigital, tal como uma "assinatura em livro de visita" daqueles que instalarem e rodarem o sketch em suas placas;
- [ ] v.1.3: utilização do FastLED v.3.3.3, ThingSpeak (receber a cor da vez "tweetada" pelo além da Internet e analisar os registros) e MQTT (idDigital e publicar registros);

- [ ] v.2: dashboard WEB (Qnt de idDigitais, corCurrent e gráfico de histórico). Esta me parece a tarefa mais complicada e que demandará maior tempo. Alguém se habilita a ajudar? :fist_right: :fist_left:
***
Vou ficar por aqui, pois já é invenção suficiente para carrear um bom trabalho (:coffee:). **Fique de olho no CheerLights!**

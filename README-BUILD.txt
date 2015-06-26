(UTF-8: Japanese)

-----------------------------------------------------------------------

eXellent Multi-platform emulator type 8 - 'XM8'
based on ePC-8801MA

Copyright (C) 2015 ＰＩ．
Version 1.00 (2015/06/07)

-----------------------------------------------------------------------

□ソースコードについて


このディレクトリには、XM8の上記バージョンをビルドするために必要となるソース
コードを収録しています。

以下プラットフォーム毎に、開発環境の構築方法およびビルド方法を記します。



□Windows


＜開発環境の構築＞

"Windows"ディレクトリにVisual Studio 2010以降のバージョンで使用可能なソリ
ューションファイル、プロジェクトファイルを収録しています。

参考として、Windowsで新たに開発環境を構築する場合の手順を以下に記します。
この場合、無償提供されているパッケージ・ファイル群のみで構築が可能です。

(1)Visual C++ 2010 Express

Visual C++ 2010 Expressをインストールします。
https://www.visualstudio.com/downloads/download-visual-studio-vs

ここでは最新版や次版のプレビューが紹介されていますが、下の方を見ていくと
2010 Expressのダウンロードリンクが残されています。


(2)Windows SDK v7.1

Windows SDK v7.1(Windows SDK for Windows 7 and .NET Framework 4)をインス
トールします。これにより(1)に64bitコンパイラを追加できます。
https://www.microsoft.com/en-us/download/details.aspx?id=8279

このインストールに失敗する場合は、以下のプログラムをアンインストールした後
に再度トライしてください。

・Visual C++ 2010 Redistributable
・Visual C++ 2010 Runtime


(3)Visual Studio 2010 Service Pack 1

Visual C++ 2010 ExpressにService Pack 1を適用します。
https://www.microsoft.com/ja-jp/download/details.aspx?id=23691

ただし、これにより(2)で追加した64bitコンパイラが削除されます。


(4)Windows SDK 7.1 用 Microsoft Visual C++ 2010 Service Pack 1
コンパイラ更新プログラム 

以下の更新プログラムを適用し、(3)で削除された64bitコンパイラを復元します。
https://www.microsoft.com/ja-jp/download/details.aspx?id=4422


(5)SDL開発ライブラリ

Windows (Visual C++ 32bit/64bit)向けライブラリをダウンロードします。
https://www.libsdl.org/download-2.0.php

これをSource\Windows\SDLへ展開します。以下のようになります。

Source\Windows\SDL\include（インクルードファイル）
Source\Widnows\SDL\lib\x86（32bit向けライブラリ）
Source\Windows\SDL\lib\x64（64bit向けライブラリ）


＜ビルド＞

Visual Studioで"XM8.sln"を開き、所望のターゲット・プラットフォームを指定し
てリビルドすると"XM8.exe"が生成されます。

Releaseターゲットはランタイムライブラリを静的リンクします。Debugターゲット
はこれと異なり、ランタイムDLLを利用する設定としています。



□Linux


＜開発環境の構築＞

g++と標準のツールチェインを利用し、SDL 2.0をビルドする環境を整えます。詳し
くは以下を参照してください。
https://wiki.libsdl.org/Installation#Linux.2FUnix

作者環境であるXUbuntu 14.04での場合を例として挙げます。

sudo apt-get install g++ build-essential libsdl2-dev


＜ビルド>

makefileを利用してビルドすればOKです。例えば以下の通りです。

cd Source
cd Linux
make clean
make



□Android


＜開発環境の構築＞

これまでAndroidアプリケーションの開発環境としてEclipse+ADT Pluginが使われて
きましたが、2015年現在はAndroid Studioへの移行が進みつつある状況です。しかし
XM8ではSDL 2.0との親和性を考慮し、従来通りのEclipse+ADTを採用しました。

通常のAndroidアプリケーションを作成する場合と同様に、以下をインストールしま
す。

(1)JDK
(2)Eclipse for Java
(3)Android SDK
(4)ADT Plugin

EclipseからAndroid SDK Managerを起動し、(3)でインストールしたAndroid SDKの
場所を指定します。さらにAndroid SDK Managerの画面で"Obsolete"にチェックし、
Android 4.0 (API 14)のSDK Platformをインストールします。

次にAndroid NDKをインストールします。これでJNI(Java Native Interface)を利用
するアプリケーションを作成できるようになります。NDKのルートディレクトリにパ
スを通し、任意のディレクトリ上で"ndk-build"が実行できることを確かめます。

最後に、SDL 2.0のソースコードをダウンロードします。（作者はSDL 2.0.3を使用し
ています）
https://www.libsdl.org/download-2.0.php

ソースコードに含まれる"Android.mk"ファイルが、Source/Android/XM8/jni/SDL/
に来るように展開します。つまり、以下の構成になります。

Source/Android/XM8/jni/SDL/Android.mk（ndk-build向けmakeファイル）
Source/Android/XM8/jni/SDL/include/*（インクルードファイル）
Source/Android/XM8/jni/SDL/src/*（SDLソースコード）


＜ビルド＞

まずJNIの部分をビルドします。以下の手順になります。

cd Source
cd Android
cd XM8
ndk-build NDK_DEBUG=0

armeabi, armeabi-v7a, x86のそれぞれについて、"libmain.so"と"libSDL2.so"が
出来ればOKです。

次にEclipseを起動し、Source/Android/XM8にあるプロジェクトをロードします。

Package Explorerの"XM8"を右クリックし、"Debug As"→"Android Application"を
選択します。ここでUSBデバッグ可能なデバイスが接続されていれば、XM8.apkが
インストール、実行されます。

このときオプションに当たるROMファイルが見つからない場合、AssetManagerで
ブレークすることがあります。実害はありませんのでEclipse上でResumeを何度か
選択し、先に進ませてください。

配布用のapkファイルを作成するには"Export Signed Application Package"操作を
行います。詳しくは以下を参照ください。
http://developer.android.com/tools/publishing/app-signing.html
(EOF)

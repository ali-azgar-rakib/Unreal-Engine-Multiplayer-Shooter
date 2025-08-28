// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterHUD.h"

void AShooterHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2D ViewpotSize;
	if (GEngine) {
		GEngine->GameViewport->GetViewportSize(ViewpotSize);

		const FVector2D ViewportCenter(ViewpotSize.X / 2.f, ViewpotSize.Y / 2.f);

		float CrosshairSpread = CrosshairSpreadMax * HUDPackage.CrosshairSpread;

		if (HUDPackage.CrosshairCenter) {
			FVector2d spread(0.f, 0.f);
			DrawCrosshair(HUDPackage.CrosshairCenter, ViewportCenter, FLinearColor::White,spread);
		}

		if (HUDPackage.CrosshairLeft) {
			FVector2d spread(- CrosshairSpread, 0.f);
			DrawCrosshair(HUDPackage.CrosshairLeft, ViewportCenter, FLinearColor::White,spread);
		}
		if (HUDPackage.CrosshairRight) {
			FVector2d spread(CrosshairSpread, 0.f);
			DrawCrosshair(HUDPackage.CrosshairRight, ViewportCenter, FLinearColor::White,spread);
		}
		if (HUDPackage.CrosshairTop) {
			FVector2D spread(0.f, -CrosshairSpread);
			DrawCrosshair(HUDPackage.CrosshairTop, ViewportCenter, FLinearColor::White,spread);
		}
		if (HUDPackage.CrosshairBottom) {
			FVector2D spread(0.f, CrosshairSpread);
			DrawCrosshair(HUDPackage.CrosshairBottom, ViewportCenter, FLinearColor::White,spread);
		}
	}

}

void AShooterHUD::DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FLinearColor Color, FVector2D spread)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();

	const FVector2D TextureDrawPoint(
		ViewportCenter.X - (TextureWidth / 2.f) + spread.X,
		ViewportCenter.Y - (TextureHeight / 2.f) + spread.Y
	);

	DrawTexture(Texture, TextureDrawPoint.X, TextureDrawPoint.Y, TextureWidth, TextureHeight, 0, 0, 1, 1, Color);
}
